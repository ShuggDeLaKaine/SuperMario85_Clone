#include "game.h"

/*! \file game.cpp
* \brief Contains functions for initialising, updating and drawing the world and all objects within.
* At this stage also contains functions that deal with resource management; textures, texts and audio.
* Also at this stage contains functions that look after and process user input.
* And finally it also contains functions that look after the player animation and movement... to be moved to player.h/cpp.
*/


//! Function to be called in main.cpp to initiate and create the entire game world.
/*!
\param - n/a
*/
Game::Game()
{
	//setting the size * origin or view, then creating the world and applying the debug draw to this world.
	view.setCenter(0.0f, 0.0f);
	view.setSize(worldSize);
	world = new b2World(gravity);
	debugDraw.setWorld(world);

	//functions to initialise all required textures, fonts, texts, sounds and vars.
	initTexture();
	initFontsTexts();
	initAudio();
	initValues();

	//function to populate the world with the required objects.
	populateWorld();

	//get bodies from moving objects that require force to be added to them.
	playerBody = playerObject[0].getBody();

	//prevent the player object from rotating.
	playerBody->SetFixedRotation(true);
	
	//grabbing and assigning the userData (for the listeners) to each object, using a pair with it's name and a void pointer.
	for (StaticRect& block : staticBlock) block.setUserData(new std::pair<std::string, void *>(typeid(decltype(block)).name(), &block));
	for (Player& player : playerObject) player.setUserData(new std::pair<std::string, void *>(typeid(decltype(player)).name(), &player));
	for (Enemy& enemy : enemyObject) enemy.setUserData(new std::pair<std::string, void*>(typeid(decltype(enemy)).name(), &enemy));
	for (Item& items : itemList) items.setUserData(new std::pair<std::string, void *>(typeid(decltype(items)).name(), &items));
	for (Obstacle& obstacles : obstaclesList) obstacles.setUserData(new std::pair<std::string, void*>(typeid(decltype(obstacles)).name(), &obstacles));
	for (StaticSensor& sensor : staticSensors) sensor.setUserData(new std::pair<std::string, void *>(typeid(decltype(sensor)).name(), &sensor));

	//setting the contact listener in the world.
	world->SetContactListener(&listener);
}

//! Function to to delete the world and set the pointer back to null.
/*!
\param - n/a
*/
Game::~Game()
{
	delete world;
	world = nullptr;
}

//! Function to draw all that is required to the desired target.
/*!
\param sf::RenderTarget target - the target is window we will be drawing this all to.
\param sf::RenderStates states - second requirement of draw(), not currently used in this function.
*/
void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	//set the view.
	target.setView(view);

	//draw background and the UI text in the scene.
	target.draw(bgPicture);

	//draw all the objects in the world.
	for (auto gBlock : staticBlock) target.draw(gBlock);
	for (auto player : playerObject) target.draw(player);
	for (auto items : itemList) target.draw(items);
	for (auto enemy : enemyObject) target.draw(enemy);
	for (auto obstacles : obstaclesList) target.draw(obstacles);
	
	//set view for UI and draw UI text.
	target.setView(uiView);
	target.draw(scoreText);
	target.draw(timerText);
	target.draw(livesText);
	target.draw(tutorialText);
	
	//check whether level is complete, if so draw the victory text too.
	if (levelComplete == true)
	{
		target.draw(victoryText1);
		target.draw(victoryText2);
	}

	//debug draw.
	if (debug == true)
		target.draw(debugDraw);
}

//! Function to update the world will all changes each step, called in main.cpp
/*!
\param float timestep - float to take the length of time for each frame.
*/
void Game::update(float timestep)
{
	//update the world.
	world->Step(timestep, velocityIterations, positionIterations);

	//checking updates on score and canJump from contact listener.
	ocl.scoreCounter(score);
	ocl.isPlayerGrounded(canJump);
	ocl.isPlayerDead(isDead);
	ocl.playAudio(playCoinSFX, playHurtSFX);

	//if player is dead, then playerDead().
	if (isDead == true)
	{
		playerDead();
	}
	//checking whether below SFXs need to be played.
	if (playCoinSFX == true)
	{
		pickUpSFX.play();
	}
	if (playHurtSFX == true)
	{
		marioHitSFX.play();
	}

	//checking to see if fallen off screen.
	fallenOffScreenCheck();

	//calling function which looks after all the impulses for player movement.
	playerMovement();

	//call function to animate the player sprite.
	animatePlayer();

	//update score, time and lives in UI.
	updateUI();

	//update all the game objects, their rendering positions.
	for (auto& player : playerObject) player.update();
	for (auto& items : itemList) items.update();
	for (auto& enemy : enemyObject) enemy.update();

	//calling function which updates the position of the camera/view to the players position but keeps in-bounds too.
	cameraController();

	//check whether current checkpoint needs updating
	checkpointMan();

	//check whether level is complete.
	completedLevel();

	//check whether game win/lose condidtions met.
	gameConditions();

	//delete debug shapes.
	if (debug == true)
		debugDraw.clear();
}

//! Function to toggle debug mode.
/*!
\param - n/a
*/
void Game::toggleDebug()
{
	debug = !debug;
}

//! Function to initialise all required textures.
/*!
\param - n/a
*/
void Game::initTexture()
{
	//reference to the PhysicalObject class.
	PhysicalObject po;
	
	//backfround texture and object.
	po.loadTexture(background, "world_01_01.png");
	bgPicture.setSize(sf::Vector2f(140, 8));
	bgPicture.setTexture(&background);
	bgPicture.setPosition(-6.0f, -3.85f);
	
	//load up ground textures.
	po.loadTexture(brick1x1, "brick_1x1_01.png");
	po.loadTexture(brick2x1, "brick_2x1_01.png");
	po.loadTexture(brick3x1, "brick_3x1_01.png");
	po.loadTexture(brick4x1, "brick_4x1_01.png");
	po.loadTexture(stones, "stone_brown_12x4.png");
	po.loadTexture(solidBlock, "hard_block_01.png");

	//load up mario player textures.
	po.loadTexture(marioIdle, "mario_idle_01.png");
	po.loadTexture(marioWalkingSpritesheet, "mario_run_spritesheet_01.png");

	//load up item textures.
	po.loadTexture(coin, "coin_01.png");
	po.loadTexture(smallTube, "tube_3x2_01.png");
	po.loadTexture(bigTube, "tube_3x4_01.png");
	po.loadTexture(flag, "victory_flag_01.png");

	//load up ememy textures.
	po.loadTexture(goombaWalkingSpriteSheet, "mushroom_spritesheet_01.png");
	po.loadTexture(goombaDead, "mushroom_dead_01.png");
}

//! Function to initialise all required fonts and texts.
/*!
\param - n/a
*/
void Game::initFontsTexts()
{
	//setting the view to take the UI input.
	uiView = sf::View(sf::Vector2f(400, 300), sf::Vector2f(800, 600));

	//load the font, throw an error and exit if fails to do so.
	if (!uiFont.loadFromFile("./assets/fonts/mario.ttf")) {
		std::cout << "Error loading UI font" << std::endl;
		exit(0);
	}

	//setting all required for score text.
	scoreText.setFont(uiFont);
	scoreText.setString("Score: 0");
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(sf::Color::Red);
	scoreText.setPosition(sf::Vector2f(20, 2));
	//for timer text.
	timerText.setFont(uiFont);
	timerText.setString("Timer: 0");
	timerText.setCharacterSize(30);
	timerText.setFillColor(sf::Color::Red);
	timerText.setPosition(320, 2);
	//for lives text.
	livesText.setFont(uiFont);
	livesText.setString("Lives: 0");
	livesText.setCharacterSize(30);
	livesText.setFillColor(sf::Color::Red);
	livesText.setPosition(650, 2);
	//for tutorial text.
	tutorialText.setFont(uiFont);
	tutorialText.setString("Press the DIRECTIONAL BUTTONS to move. \nThe SPACE BAR to jump. \nAnd M to un/mute.");
	tutorialText.setCharacterSize(22);
	tutorialText.setFillColor(sf::Color::Red);
	tutorialText.setPosition(20, 50);
	//for victory1 text.
	victoryText1.setFont(uiFont);
	victoryText1.setString("");
	victoryText1.setCharacterSize(50);
	victoryText1.setFillColor(sf::Color::Red);
	victoryText1.setPosition(100, 150);
	//for victory2 text.
	victoryText2.setFont(uiFont);
	victoryText2.setString("");
	victoryText2.setCharacterSize(40);
	victoryText2.setFillColor(sf::Color::Red);
	victoryText2.setPosition(250, 250);
	//for game over text.
	gameOverText.setFont(uiFont);
	gameOverText.setString("");
	gameOverText.setCharacterSize(50);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setPosition(400, 300);
}

//! Function to initalise all required audio.
/*!
\param - n/a
*/
void Game::initAudio()
{
	//open up the music track required.
	if (!mainMarioMusic.openFromFile("./assets/audio/Main_Theme_01.ogg"))
	{
		std::cout << "Error loading Main Theme music track" << std::endl;
		exit(0);
	}

	//load up required short clips required, exit if fail to do so.
	if (!marioJumpBuffer.loadFromFile("./assets/audio/Jump_01.wav")) {
		std::cout << "Error loading Mario Jump sound file" << std::endl;
		exit(0);
	}
	if (!marioHitBuffer.loadFromFile("./assets/audio/Hit_01.wav")) {
		std::cout << "Error loading Mario Hit sound file" << std::endl;
		exit(0);
	}
	if (!marioDeadBuffer.loadFromFile("./assets/audio/PlayerDead_01.wav")) {
		std::cout << "Error loading Mario Dead sound file" << std::endl;
		exit(0);
	}
	if (!pickUpBuffer.loadFromFile("./assets/audio/PickUp_01.wav")) {
		std::cout << "Error loading Item Pick-Up sound file" << std::endl;
		exit(0);
	}
	
	//set the relevant sound buffers to the SFXs.
	marioJumpSFX.setBuffer(marioJumpBuffer);
	marioHitSFX.setBuffer(marioHitBuffer);
	marioDeadSFX.setBuffer(marioDeadBuffer);
	pickUpSFX.setBuffer(pickUpBuffer);
	
}

//! Function to initalise all required var values.
/*!
\param - n/a
*/
void Game::initValues()
{
	//start game with the player stopped and moving directions false.
	playerStop = true;
	isWalking = false;
	movingRight = false;
	movingLeft = false;
	rightLast = false;

	//initialising score, time and lives text.
	score = 0;
	lives = 3;
	totalTime = 180.0f;
	currentTime = totalTime;

	//initialising bools for jumping and player dead.
	canJump = false;
	isDead = false;
	levelComplete = false;

	//set the music to playing and bool to true.
	mainMarioMusic.play();
	musicPlaying = true;
	bool playCoinSFX = false;
	bool playHurtSFX = false;

	//setting IntRect for looking left and right, to flip mario sprite to look in direction of movement.
	lookRight = sf::IntRect(0, 0, 50, 50);
	lookLeft = sf::IntRect(50, 0, -50, 50);

	//init start co-ords in world and standard block size and the end co-ords x-pos.
	startPosition = b2Vec2(-3.0f, 2.0f);
	endPosition = 125.0f;
	//initialising positions of checkpoints and setting current checkpoint location.
	currentCheckpoint = startPosition;
	checkpoint1 = 25.0f;
	checkpoint2 = 50.0f;
	checkpoint3 = 75.0f;
	checkpoint4 = 100.0f;
}

//! Function to update the UI text elements with changes.
/*!
\param - n/a
*/
void Game::updateUI()
{
	//getting a clock timer, taking that away from totalTime and then setting to int to be displayed in UI.
	uiElapsedTime = timerClock.getElapsedTime().asSeconds();

	//checking whether level is complete, is so then pause the current time.
	if (levelComplete == false)
	{
		currentTime = totalTime - uiElapsedTime;
	}
	else
	{
		currentTime = currentTime;
	}

	//convert from float to int to add to string.
	int i_time = (int)currentTime;

	//setting int vars to strings.
	std::string currentScore = "Score: " + std::to_string(score);
	std::string currentLives = "Lives: " + std::to_string(lives);
	std::string currentTime;

	//if time drops below 0, set string to "times up".
	if(i_time > 0)
	{
		currentTime = "Time: " + std::to_string(i_time);
	}
	else
	{
		currentTime = "Time: Time is up!";
	}

	//setting above strings to to the UI texts.
	scoreText.setString(currentScore);
	timerText.setString(currentTime);
	livesText.setString(currentLives);
}

//! Function to take and process all inputted keys from the user and give desired actions to those inputs.
/*!
\param sf::Keyboard::Key key - taking the keyboard input from the user.
*/
void Game::userInput(sf::Keyboard::Key key)
{
	//vec2 to take current position, used for debugging and world checking.
	b2Vec2 currentPosition = playerBody->GetPosition();

	//key input so player is moving, therefore playerStop should be false;
	playerStop = false;

	//input for moving right and left, jumping and default for none movement.
	//some additional key but for debugging and easier for presentation.
	switch (key)
	{
	case sf::Keyboard::Tab:
		toggleDebug();
	case sf::Keyboard::Right:
		movingRight = true;
		movingLeft = false;
		break;
	case sf::Keyboard::Left:
		movingLeft = true;
		movingRight = false;
		break;
	case sf::Keyboard::Space:
		playerJump();
		break;
	case sf::Keyboard::E:
		//debug to take player to the end of level, so can show the end 'Victory' condition.
		playerBody->SetTransform(b2Vec2(120.0f, 3.0f), 0.0f);
		break;
	case sf::Keyboard::S:
		//debug to move player through level, to check world and show examples of play.
		playerBody->SetTransform(b2Vec2((currentPosition.x + 10.0f), 3.0f), 0.0f);
		break;
	case sf::Keyboard::P:
		//debug to get x-pos of mario 
		std::cout << "Mario x position is: " << playerBody->GetPosition().x << std::endl;
		break;
	case sf::Keyboard::M:
		muteMusic();
		break;
	default:
		movingRight = false;
		movingLeft = false;
		playerStop = true;
		break;
	}
}

//! Function to toggle the background musice on and off.
/*!
\param - n/a
*/
void Game::muteMusic()
{
	//little function to check whether music is muted or not and flips it on or off as required.
	if (musicPlaying == true)
	{
		mainMarioMusic.pause();
		musicPlaying = false;
	}
	else if (musicPlaying == false)
	{
		mainMarioMusic.play();
		musicPlaying = true;
	}
	else
	{
		std::cout << "issue with music track, musicPlaying is neither true nor false" << std::endl;
	}
}

//! Function to give the desired limited force on the player object to create motion.
/*!
\param - n/a
*/
void Game::playerMovement()
{
	//get the current velocity of the body on the player object.
	playerVelocity = playerBody->GetLinearVelocity();
	desiredVelocity = 0.0f;

	//checking which direction player is moving and applying forces in that direction. Forces capped at +/- 3.0f to give a top speed.
	if (movingRight == true)
	{
		desiredVelocity = b2Min(playerVelocity.x + 0.1f, 3.0f);
	}
	else if (movingLeft == true)
	{
		desiredVelocity = b2Max(playerVelocity.x - 0.1f, -3.0f);
	}
	else if (playerStop == true)
	{
		desiredVelocity = playerVelocity.x * -0.001f;
	}
	
	//boundary on the left side of screen, stops player unless moving right, back into the scene.
	if (playerBody->GetPosition().x < -5.0f)
	{
		desiredVelocity = playerVelocity.x * 0.001f;
		if (movingRight == true)
		{
			desiredVelocity = b2Min(playerVelocity.x + 0.1f, 3.0f);
		}
	}

	//working out the required change in velocity, multiply by objects mass to get impulse and applying to the x-axis.
	velocityChange = desiredVelocity - playerVelocity.x;
	impulse = playerBody->GetMass() * velocityChange; //disregard time factor.
	playerBody->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0.0f), true);
}

//! Function to give the required forces to have the player object 'jump'.
/*!
\param - n/a
*/
void Game::playerJump()
{
	//check whether player is grounded and can jump, if so apply jump impulse on the y-axis but keep the x-axis forces that are already in play.
	if (canJump == true)
	{
		float currentXImpulse = playerBody->GetLinearVelocity().x;
		impulse = playerBody->GetMass() * 10;
		playerBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, impulse), true);
		marioJumpSFX.play();
	}
}

//! Function on key release to stop the players movement, with the exception of when in the air.
/*!
\param sf::Keyboard::Key key - used to see if the key released was SPACE, if so, do not stop the movement of player object when in air.
*/
void Game::stopMovement(sf::Keyboard::Key key)
{
	//if the released key is jump, then must be Left or Right, therefore stop movement.
	if (key != sf::Keyboard::Space)
	{
		playerStop = true;
		movingRight = false;
		movingLeft = false;
	}
}

//! Function to animate the player object when it is in motion.
/*!
\param - n/a
*/
void Game::animatePlayer()
{
	//keeping a clock going through out scene.
	elapsedTime = marioClock.getElapsedTime().asSeconds();

	isWalking = isMoving();
	if (isWalking == true)
	{
		//set the sprite for the player to be the walking sprite and looped.
		playerObject[0].setTexture(&marioWalkingSpritesheet);
		if (movingRight == true)
		{
			//flip to have sprite looking to the right direction.
			playerObject[0].setTextureRect(lookRight);
			//get animating...
			if (elapsedTime > 0.5f)
			{
				if (lookRight.left == 150){
					lookRight.left = 0;
				} else {
					lookRight.left += 50;
				}
				playerObject[0].setTextureRect(lookRight);
				marioClock.restart();
			}
			rightLast = true;
		}
		else if (movingLeft == true)
		{
			//flip to have sprite looking to the left direction.
			playerObject[0].setTextureRect(lookLeft);
			//and animation loop through spritesheet.
			if (elapsedTime > 0.5f)
			{
				if (lookLeft.left == 200) {
					lookLeft.left = 50;
				} else {
					lookLeft.left += 50;
				}
				playerObject[0].setTextureRect(lookLeft);
				marioClock.restart();
			}
			rightLast = false;
		}
	}
	//else if stopped, make sure the sprite is facing the last direction of movement.
	else if (isWalking == false)
	{
		if (rightLast == true) {
			playerObject[0].setTextureRect(sf::IntRect(0, 0, 50, 50));
		}
		else if (rightLast == false) {
			playerObject[0].setTextureRect(sf::IntRect(50, 0, -50, 50));
		}
		playerObject[0].setTexture(&marioIdle);
	}
}

//! Function to see if player has fallen off the screen and all playerDead() if so.
/*!
\param - n/a
*/
void Game::fallenOffScreenCheck()
{
	//get the players y-axis and check whether it's more than 12, if so, then fallen off the screen.
	float playerYPos = playerBody->GetPosition().y;
	if (playerYPos >= 12.0f)
	{
		playerDead();
	}
}

//! Function to run on the player objects 'death'.
/*!
\param - n/a
*/
void Game::playerDead()
{
	//play mario death sfx.
	marioDeadSFX.play();

	//take a life off the player.
	lives--;

	//send player to its last checkpoint spawn position and set impulses to 0 on body.
	playerBody->SetTransform(currentCheckpoint, 0.0f);
	playerBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

	//movement bools set so idle.
	playerStop = true;
	movingRight = false;
	movingLeft = false;
}

//! Function to link the camera to the motion of the player object but keep it constrained to the view.
/*!
\param - n/a
*/
void Game::cameraController()
{
	//setting camera to follow the player.
	float yOffset = 0.65f;
	float xBoundary = 0.0f;
	view.setCenter(sf::Vector2f(playerBody->GetPosition().x, yOffset));

	//setting a check as to camera position to out-of-world boundaries; keeps camera in world.
	if (view.getCenter().x < xBoundary)
		view.setCenter(sf::Vector2f(xBoundary, yOffset));
}

//! Function to check to see whether the player object is moving.
/*!
\param - n/a
*/
bool Game::isMoving()
{
	//set a bool for moving and get the movement force on the x-axis.
	bool moving;
	float movementForceX = playerBody->GetLinearVelocity().x;
	
	//if no force on the x-axis, mustn't be moving left nor right; set moving bool.
	if (movementForceX != 0) {
		moving = true;
	} else {
		moving = false;
	}
	//return the bool moving.
	return moving;
}

//! Function to see whether the player has completed the level.
/*!
\param - n/a
*/
void Game::completedLevel()
{
	float currentPos = playerBody->GetPosition().x;
	if (currentPos >= endPosition)
	{
		levelComplete = true;
		//stop player movement

	}
}

//! Function to update the respawn checkpoint throughout the level depending on how far the player has gotten.
/*!
\param - n/a
*/
void Game::checkpointMan()
{
	float currentPlayerPosition = playerBody->GetPosition().x;
	//checking current player position to the checkpoints, as player passes them that checkpoint is set to the respawn checkpoint.
	if (currentPlayerPosition >= checkpoint1 && currentPlayerPosition < checkpoint2 &&
		currentPlayerPosition < checkpoint3 && currentPlayerPosition < checkpoint4) {
		currentCheckpoint.x = checkpoint1;
	} else if (currentPlayerPosition >= checkpoint2 && currentPlayerPosition < checkpoint3 &&
		currentPlayerPosition < checkpoint4) {
		currentCheckpoint.x = checkpoint2;
	} else if (currentPlayerPosition >= checkpoint3 && currentPlayerPosition < checkpoint4) {
		currentCheckpoint.x = checkpoint3;
	} else if (currentPlayerPosition >= checkpoint4) {
		currentCheckpoint.x = checkpoint4;
	}
}

//! Function to see whether game end conditions, victory or defeat, have been met.
/*!
\param - n/a
*/
void Game::gameConditions()
{
	//no more lives, LOSE game.
	if (lives <= 0)
	{
		gameOver = true;
	}

	//time has ran out, LOSE game.
	if (currentTime <= 0.0f)
	{
		gameOver = true;
	}

	//got to end of level, WON game.
	if (levelComplete == true)
	{
		gameOver = true;

		//draw WIN text.
		victoryText1.setString("WELCOME YOU HAVE WON!");
		std::string finalScore = "Final Score is: " + std::to_string(score);
		victoryText2.setString(finalScore);
	}
}

//! Function to populate the world with all required objects.
/*!
\param - n/a
*/
void Game::populateWorld()
{
	//adding a sensor object into world, to be used by contact listener.
	staticSensors.push_back(StaticSensor(world, sf::Vector2f(-3.0f, -3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f));

	//add the player to the world.
	playerObject.resize(1);
	playerObject[0] = Player(world, sf::Vector2f(-3.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::PLAYER,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::ITEM | PhysicalObject::CollisionFilter::OBSTACLE, &marioWalkingSpritesheet, &marioSprite, 4, 0.25f);

	//adding the enemies into the world.
	enemyObject.resize(12);
	enemyObject[0] = Enemy(world, sf::Vector2f(1.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[1] = Enemy(world, sf::Vector2f(14.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[2] = Enemy(world, sf::Vector2f(19.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[3] = Enemy(world, sf::Vector2f(24.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[4] = Enemy(world, sf::Vector2f(28.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[5] = Enemy(world, sf::Vector2f(38.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[6] = Enemy(world, sf::Vector2f(53.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[7] = Enemy(world, sf::Vector2f(67.5f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[8] = Enemy(world, sf::Vector2f(80.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[9] = Enemy(world, sf::Vector2f(91.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[10] = Enemy(world, sf::Vector2f(106.5f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);
	enemyObject[11] = Enemy(world, sf::Vector2f(116.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ENEMY,
		PhysicalObject::CollisionFilter::GROUND | PhysicalObject::CollisionFilter::BOUNDARY | PhysicalObject::CollisionFilter::BREAKABLE_BOX |
		PhysicalObject::CollisionFilter::UNBREAKABLE_BOX | PhysicalObject::CollisionFilter::CHEST_BOX | PhysicalObject::CollisionFilter::ENEMY |
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::OBSTACLE, &goombaWalkingSpriteSheet, &goombaSprite, 2, 1.0f);

	//adding items into the world.
	itemList.resize(45);
	itemList[0] = Item(world, sf::Vector2f(0.5f, 1.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[1] = Item(world, sf::Vector2f(1.5f, 1.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[2] = Item(world, sf::Vector2f(4.5f, 0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[3] = Item(world, sf::Vector2f(7.5f, -1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[4] = Item(world, sf::Vector2f(8.0f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[5] = Item(world, sf::Vector2f(9.0f, 1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[6] = Item(world, sf::Vector2f(13.0f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[7] = Item(world, sf::Vector2f(14.0f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[8] = Item(world, sf::Vector2f(18.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[9] = Item(world, sf::Vector2f(19.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[10] = Item(world, sf::Vector2f(22.0f, 0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[11] = Item(world, sf::Vector2f(22.0f, 0.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[12] = Item(world, sf::Vector2f(22.0f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[13] = Item(world, sf::Vector2f(23.5f, -1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[14] = Item(world, sf::Vector2f(24.5f, -1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[15] = Item(world, sf::Vector2f(27.0f, -2.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[16] = Item(world, sf::Vector2f(38.5f, 1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[17] = Item(world, sf::Vector2f(41.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[18] = Item(world, sf::Vector2f(44.0f, -1.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[19] = Item(world, sf::Vector2f(47.5f, -1.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[20] = Item(world, sf::Vector2f(48.5f, -1.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[21] = Item(world, sf::Vector2f(52.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[22] = Item(world, sf::Vector2f(53.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[23] = Item(world, sf::Vector2f(64.0f, 0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[24] = Item(world, sf::Vector2f(64.0f, 0.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[25] = Item(world, sf::Vector2f(64.0f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[26] = Item(world, sf::Vector2f(65.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[27] = Item(world, sf::Vector2f(66.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[28] = Item(world, sf::Vector2f(72.5f, 1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[29] = Item(world, sf::Vector2f(73.5f, 0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[30] = Item(world, sf::Vector2f(74.5f, 1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[31] = Item(world, sf::Vector2f(79.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[32] = Item(world, sf::Vector2f(80.5f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[33] = Item(world, sf::Vector2f(96.0f, 0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[34] = Item(world, sf::Vector2f(96.0f, 0.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[35] = Item(world, sf::Vector2f(96.0f, -0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[36] = Item(world, sf::Vector2f(107.0f, 0.5f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[37] = Item(world, sf::Vector2f(104.5f, -1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[38] = Item(world, sf::Vector2f(103.5f, -1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[39] = Item(world, sf::Vector2f(115.5f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[40] = Item(world, sf::Vector2f(116.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[41] = Item(world, sf::Vector2f(116.5f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[42] = Item(world, sf::Vector2f(117.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[43] = Item(world, sf::Vector2f(117.5f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);
	itemList[44] = Item(world, sf::Vector2f(118.0f, 3.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::ITEM,
		PhysicalObject::CollisionFilter::PLAYER, &coin);

	//adding obstacles into the world.
	obstaclesList.resize(14);
	obstaclesList[0] = Obstacle(world, sf::Vector2f(12.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[1] = Obstacle(world, sf::Vector2f(16.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[2] = Obstacle(world, sf::Vector2f(36.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[3] = Obstacle(world, sf::Vector2f(48.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[4] = Obstacle(world, sf::Vector2f(71.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[5] = Obstacle(world, sf::Vector2f(76.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[6] = Obstacle(world, sf::Vector2f(103.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[7] = Obstacle(world, sf::Vector2f(110.0f, 2.8f), sf::Vector2f(1.5f, 1.0f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &smallTube);
	obstaclesList[8] = Obstacle(world, sf::Vector2f(22.0f, 2.38f), sf::Vector2f(1.5f, 1.8f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &bigTube);
	obstaclesList[9] = Obstacle(world, sf::Vector2f(30.0f, 2.38f), sf::Vector2f(1.5f, 1.8f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &bigTube);
	obstaclesList[10] = Obstacle(world, sf::Vector2f(51.0f, 2.38f), sf::Vector2f(1.5f, 1.8f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &bigTube);
	obstaclesList[11] = Obstacle(world, sf::Vector2f(64.0f, 2.38f), sf::Vector2f(1.5f, 1.8f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &bigTube);
	obstaclesList[12] = Obstacle(world, sf::Vector2f(83.0f, 2.38f), sf::Vector2f(1.5f, 1.8f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &bigTube);
	obstaclesList[13] = Obstacle(world, sf::Vector2f(96.0f, 2.38f), sf::Vector2f(1.5f, 1.8f), 0.0f, PhysicalObject::CollisionFilter::OBSTACLE,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &bigTube);

	//adding ground blocks into the world.
	staticBlock.resize(27);
	staticBlock[0] = StaticRect(world, sf::Vector2f(0.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[1] = StaticRect(world, sf::Vector2f(14.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[2] = StaticRect(world, sf::Vector2f(26.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[3] = StaticRect(world, sf::Vector2f(38.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[4] = StaticRect(world, sf::Vector2f(52.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[5] = StaticRect(world, sf::Vector2f(66.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[6] = StaticRect(world, sf::Vector2f(78.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[7] = StaticRect(world, sf::Vector2f(92.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[8] = StaticRect(world, sf::Vector2f(106.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[9] = StaticRect(world, sf::Vector2f(118.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[10] = StaticRect(world, sf::Vector2f(130.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);
	staticBlock[11] = StaticRect(world, sf::Vector2f(142.0f, 4.29f), sf::Vector2f(12.0f, 2.0f), 0.0f, PhysicalObject::CollisionFilter::GROUND,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &stones);

	//adding the platforms
	staticBlock[12] = StaticRect(world, sf::Vector2f(1.0f, 2.0f), sf::Vector2f(2.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick4x1);
	staticBlock[13] = StaticRect(world, sf::Vector2f(4.5f, 1.0f), sf::Vector2f(1.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick3x1);
	staticBlock[14] = StaticRect(world, sf::Vector2f(13.5f, 0.0f), sf::Vector2f(2.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick4x1);
	staticBlock[15] = StaticRect(world, sf::Vector2f(19.0f, 0.0f), sf::Vector2f(1.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick3x1);
	staticBlock[16] = StaticRect(world, sf::Vector2f(24.0f, -0.5f), sf::Vector2f(2.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick4x1);
	staticBlock[17] = StaticRect(world, sf::Vector2f(27.0f, -2.0f), sf::Vector2f(1.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick2x1);
	staticBlock[18] = StaticRect(world, sf::Vector2f(38.5f, 1.5f), sf::Vector2f(1.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick3x1);
	staticBlock[19] = StaticRect(world, sf::Vector2f(48.0f, -1.0f), sf::Vector2f(1.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick3x1);
	staticBlock[20] = StaticRect(world, sf::Vector2f(53.0f, 0.0f), sf::Vector2f(2.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick4x1);
	staticBlock[21] = StaticRect(world, sf::Vector2f(66.0f, 0.0f), sf::Vector2f(2.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick4x1);
	staticBlock[22] = StaticRect(world, sf::Vector2f(80.0f, 0.0f), sf::Vector2f(2.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick4x1);
	staticBlock[23] = StaticRect(world, sf::Vector2f(104.0f, -0.5f), sf::Vector2f(1.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick3x1);
	staticBlock[24] = StaticRect(world, sf::Vector2f(107.0f, 1.0f), sf::Vector2f(1.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick2x1);
	staticBlock[25] = StaticRect(world, sf::Vector2f(41.5, 0.0f), sf::Vector2f(1.0f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick2x1);
	staticBlock[26] = StaticRect(world, sf::Vector2f(44.0f, -1.0f), sf::Vector2f(0.5f, 0.5f), 0.0f, PhysicalObject::CollisionFilter::BREAKABLE_BOX,
		PhysicalObject::CollisionFilter::PLAYER | PhysicalObject::CollisionFilter::ENEMY, &brick1x1);
}