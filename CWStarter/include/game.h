#pragma once
/*!
\file game.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>

#include "SFMLDebugDraw.h"
#include "dynamicCircle.h"
#include "dynamicRect.h"
#include "staticRect.h"
#include "staticSensor.h"
#include "player.h"
#include "item.h"
#include "enemy.h"
#include "obstacle.h"
#include "ObjectContactListener.h"

/*! \class Game
\brief All the info about the game; all the objects, rendering and updating the world.
*/

class Game : public sf::Drawable {
private:
	sf::View view;			//!<  this view maps from the physical co-ords to the rendering co-ords.
	sf::Vector2f worldSize = sf::Vector2f(12.0f, 8.0f);		//!< size of this world is 10 by 6 metres.
	sf::Event keyEvent;		//!< to take the Event of a user/key interaction.

	ObjectContactListener ocl;			//!< reference to class ObjectContactListener.
	Player pc;							//!< reference to class Player.
	ObjectContactListener listener;		//<! object for the in world listening object, for object collisions.
	
	b2World* world = nullptr;			//!< pointer the Box2D world.
	const int velocityIterations = 7;	//!< each update 7 velocity iterations/corrections within the physics engine.
	const int positionIterations = 5;	//!< each update 3 position iterations/corrections.
	const b2Vec2 gravity = b2Vec2(0.0f, 9.81f);		//!< standard earth gravity to be used in world. (REMEMBER TO DEDUCT THIS VALUE)

	bool debug = false;			//!< toggle for debug drawing.
	SFMLDebugDraw debugDraw;	//!< Box2D debug drawing.

	sf::Clock marioClock;		//!< clock to track the time of animations.
	float elapsedTime;			//!< float to take elapsed time between animations.
	sf::IntRect lookRight;		//!< IntRect for spritesheet, used to flip mario to look Right.
	sf::IntRect lookLeft;		//!< IntRect for spritesheet, used to flip mario to look Left.
	sf::IntRect lookIdle;		//!< IntRect for spritesheet, used to to keep mario looking in the last direction he moved when idle.

	std::vector<StaticRect> staticBlock;		//!< static rectangles for ground blocks.
	std::vector<Obstacle> obstaclesList;		//!< static rects for in game obstacles.
	std::vector<Player> playerObject;			//!< dynamic rectangle for player object.
	std::vector<Enemy> enemyObject;				//!< dynamic rectangle for the enemy objects.
	std::vector<StaticSensor> staticSensors;	//!< for the in world static sensors.
	std::vector<Item> itemList;					//!< list for items in the world.

	b2Body* playerBody;		//!< pointer to the body element of player object; that we'll apply forces to.
	b2Body* goombaBody;		//!< pointer to the body element of an enemy objec; that we'll apply forces to.
	b2Body* turtleBody;		//!< pointer to the body element of the turtle enemy objects. 

	bool isWalking;			//!< bool if the player in moving in any direction.
	bool movingRight;		//!< bool if player is moving right.
	bool movingLeft;		//!< bool if player is moving left.
	bool rightLast;			//!< bool to take last direction player was facing.
	bool canJump;			//!< bool if player is on the ground and therefore can jump.
	bool playerStop;		//!< bool whether player should stop.
	bool isDead;			//!< bool checking whether the player is dead or not.

	b2Vec2 startPosition;	//!< vec2 to hold co-ords for start position in scene. 
	b2Vec2 currentCheckpoint;	//!< vec2 to contain co-ords location of current checkpoint to respawn.
	float checkpoint1;		//!< float for x-axis position of checkpoint 1.
	float checkpoint2;		//!< float for x-axis position of checkpoint 2.
	float checkpoint3;		//!< float for x-axis position of checkpoint 3.
	float checkpoint4;		//!< float for x-axis position of checkpoint 4.
	float endPosition;		//!< float to hold value for x-axis of end position in scene.

	b2Vec2 playerVelocity;	//!< to hold the current velocity of player.
	float desiredVelocity;	//!< the desired velocity.
	float velocityChange;	//!< change of velocity required.
	float impulse;			//!< strength of impulse to be applied as a force on a body.

	void initTexture();		//!< function to initialise all the required textures.
	void initFontsTexts();	//!< function to initialise all required for fonts/text.
	void initAudio();		//!< function to initialise all the audio files required.
	void initValues();		//!< function to initialise all necessary vars.
	void populateWorld();	//!< function to populate the world with all required objects.
	void animatePlayer();	//!< function to animate the player object with its spritesheet.
	void cameraController();//!< function to control the position and boundaries for the camera/view of world.
	void muteMusic();		//!< function to mute/unmute music.
	bool isMoving();		//!< bool function to check whether player is moving or not.
	void completedLevel();	//!< function to check whether player has completed the level.
	void checkpointMan();	//!< function to handle checkpoint 
	void gameConditions();	//!< function to check whether player has won or lost the game.

	bool levelComplete;		//!< bool for whether player has completed the level.
	int score;				//!< int to take the player score.
	int lives;				//!< int to take the number of lives.
	float currentTime;		//!< float to take the time for a time counter.
	float totalTime;		//!< float to take value of the total time.
	sf::Clock timerClock;	//!< clock to be used in UI timer.
	float uiElapsedTime;	//!< float to take the value for elapsed time.

	sf::View uiView;		//!< view to be used to draw/display UI text.
	sf::Font uiFont;		//!< font to take the font file for the UI text.
	sf::Text scoreText;		//!< text to take the text information for the UI.
	sf::Text timerText;		//!< text to take text info for a game timer.
	sf::Text livesText;		//!< text to take the info for the number of lives the player has.
	sf::Text tutorialText;	//!< text to take info for the tutorial.
	sf::Text victoryText1;	//!< text to take victory message text.
	sf::Text victoryText2;	//!< text to take victory message text.
	sf::Text gameOverText;	//!< text to take game over message text.

	sf::RectangleShape bgPicture; //!< rectangle shape to hold the background image.
	sf::Texture background;	//!< texture for background image.
	sf::Texture brick1x1;	//!< texture for brick 1x1.
	sf::Texture brick2x1;	//!< texture for brick 2x1.
	sf::Texture brick3x1;	//!< texture for brick 3x1.
	sf::Texture brick4x1;	//!< texture for brick 4x1.
	sf::Texture solidBlock;	//!< texture for solid block.
	sf::Texture stones;		//!< texture for stones.
	sf::Texture coin;		//!< texture for coins.
	sf::Texture smallTube;	//!< texture for small tube.
	sf::Texture bigTube;	//!< texture for big tube.
	sf::Texture flag;		//!< texture for the victory flag.

	sf::Sprite goombaSprite;	//!< sprite to take mushroom walking spritesheet.
	sf::Texture goombaWalkingSpriteSheet;	//!< texture for mushroom walking spritesheet.
	sf::Texture goombaDead;	//!< texture for mushroom dead.

	sf::Sprite marioSprite;	//!< sprite for mario walking spritesheet.
	sf::Texture marioIdle;	//!< texture for idle mario.
	sf::Texture marioWalkingSpritesheet;	//!< texture of mario walking spritesheet.
	sf::Texture marioDeadSpritesheet;		//!< texture of mario dead spritesheet.
	sf::Texture marioJumpingSpritesheet;	//!< texture of mario jumping spritesheet.

	sf::Music mainMarioMusic;			//!< contains main mario music.
	sf::SoundBuffer marioJumpBuffer;	//!< contains audio data for mario jump.
	sf::SoundBuffer marioHitBuffer;		//!< contains audio data for mario hit.
	sf::SoundBuffer marioDeadBuffer;	//!< contains audio data for mario death.
	sf::SoundBuffer pickUpBuffer;		//!< contains audio data for item pick up.
	sf::Sound marioJumpSFX;				//!< sound reference for jump.
	sf::Sound marioHitSFX;				//!< sound reference for hit.
	sf::Sound marioDeadSFX;				//!< sound reference for death.
	sf::Sound pickUpSFX;				//!< sound reference for pick up.

	bool musicPlaying;				//!< bool as to whether the music is playing.
	bool playCoinSFX;				//!< bool as to whether coin SFX needs to be played.
	bool playHurtSFX;				//!< bool as to whether enemy hurt SFX needs to be played.
	void playerMovement();			//!< function to apply forces to player body for movement.
	void playerJump();				//!< function to apply impulse to the y-axis of the player body.
	void fallenOffScreenCheck();	//!< function to check whether the player has fallen out of the scene.
	void playerDead();				//!< function for player death, applies required changes to world.
	void updateUI();				//!< function to update the UI text elements; score, time, lives etc.

public:
	Game();		//!< constructor to setup the game.
	~Game();	//!< deconstructor to delete and clean up pointers.

	void update(float timestep);	//!< update the game with the given timestep.
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;	//!< draw the game to the render context.
	void toggleDebug();				//!< toggles debug drawing.
	void userInput(sf::Keyboard::Key key);		//!< user keyboard input to control player object movement.
	void stopMovement(sf::Keyboard::Key key);	//!< function to stop forces applied to player body.
	bool gameOver;					//!< bool for whether the gameOver parameters have been met.
};