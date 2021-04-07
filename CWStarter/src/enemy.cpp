#include "enemy.h"

/*! \file enemy.cpp
* \brief Contains function to create an enemy object within the world.
* Functions to update the enemy object within the world.
* Also contains functions that deal with enemy movement, range, direction and collision.
*/

//! Function to create a enemy object within the required world.
/*!
\param b2World world - the world to create this object in.
\param sf::Vector2f position - a Vec2f of the desired position co-ords in world.
\param sf::Vector2f position - a Vec2f of the desired size of object.
\param float orientation - a float for the required orientation of the object in world.
\param uint16 categoryBits - uint16 for physicalObject.h CollisionFilter enum, for what type object this is.
\param uint16 maskBits -  - uint16 for physicalObject.h CollisionFilter enum, for what types object this will collide with.
\param sf::Texture texture - an sfml reference to Texture to contain what texture this object should have.
\param sf::Sprite sprite - the sprite container to attach the texture too, used for spritesheeting animations.
\param int frames - int for how many frames of animation are on the sprite sheet.
\param float animDur - float for the time length required for each frame.
*/
Enemy::Enemy(b2World* world, const sf::Vector2f& position, const sf::Vector2f size, float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture, sf::Sprite* sprite, int frames, float animDur)
{
	//setting the texture for the object from the texture provided in the parameters.
	setTexture(texture);
	//setting the frame size for each frame of text spritesheet. 
	sf::IntRect rectSprite(0, 0, 50, 50);
	setTextureRect(rectSprite);
	enemySprite = *sprite;
	numFrames = frames;
	animDuration = animDur;

	//the body element of object, setting its position, orientation and type.
	b2BodyDef bodyDef;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = orientation * DEG2RAD;
	bodyDef.type = b2_dynamicBody;

	//creating a body in the world and assigning the above attributes.
	body = world->CreateBody(&bodyDef);
	body->SetUserData(this); // used by our contact listener

	//the shape element of the object, circle so a radius.
	b2PolygonShape shape;
	shape.SetAsBox(size.x * 0.5f, size.y * 0.5f);
	shape.m_radius = 0.0f;

	//the fixture element, to take vars in PhysicalObject for mass, friction and bouncyness of object. And its shape.
	b2FixtureDef fixtureDef;
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.shape = &shape;

	//setting the category and mask of this objects collisions.
	fixtureDef.filter.categoryBits = categoryBits;
	fixtureDef.filter.maskBits = maskBits;

	//attaching the fixture to the body of the object.
	body->CreateFixture(&fixtureDef);

	//using some SFML functions to set the position of the object within the scene.
	setPosition(position);
	setSize(size);
	setOrigin(size * 0.5f);
	setRotation(orientation);
	setOutlineThickness(0.0f);

	//set enemies to have a fixed rotation.
	body->SetFixedRotation(true);

	//initialise toRemove & changeDirection bools to false.
	toRemove = false;
	changeDirection = false;

	//initialising the object pool locations.
	b_objectPool = b2Vec2(-12.0f, -10.0f);
	sf_objectPool = sf::Vector2f(-12.0f, -10.0f);

	//initialise movingRight to false, so enemies with all start with a move to the left, towards the player.
	movingRight = false;

	//initialising the range of movement for enemy object.
	movementRange = 3.0f;
	rightRange = position.x + movementRange;
	leftRange = position.x - movementRange;
}

//! Function to remove body, and therefore object, from world it is in.
/*!
/params - n/a
*/
Enemy::~Enemy()
{
	//get this body from world and destroy it from world.
	//body->GetWorld()->DestroyBody(body);
}

//! Function to update the position and rotation of this object within its world, applying the movement function. As well as check update it's collision, range and removal checks.
/*!
\param - n/a
*/
void Enemy::update()
{
	//updating position of the object.
	b2Vec2 position = body->GetPosition();
	setPosition(position.x, position.y);
	float angle = body->GetAngle() * RAD2DEG;
	setRotation(angle);

	//check enemy has collided with obstacle, then within its movement range and then apply relevant movement.
	obstacleCollisionCheck();
	rangeCheck();
	enemyMovement();

	//check whether enemy object needs to be deactivated and pooled.
	toBeRemovedCheck();
}

//! Function to to check whether toRemove bool is true and then deactive object and put back into its object pool location.
/*!
\param n/a
*/
void Enemy::toBeRemovedCheck()
{
	//if objectContactListener sets toRemove to true, then deactive body and move to object pool.
	if (toRemove == true)
	{
		body->SetActive(false);
		body->SetTransform(b2Vec2(b_objectPool), 0.0f);
		setPosition(sf_objectPool);
	}
}

//! Function to apply a limited force on the x-axis in the required direction of movement.
/*!
\param - n/a
*/
void Enemy::enemyMovement()
{
	//grab current velocity and initial desired velocity to 0.
	enemyVelocity = body->GetLinearVelocity();
	desiredVelocity = 0.0f;

	//change desired velocity to have a max of 1.2f and +/- by 0.1f on current velocity until at the max of 1.2f.
	//checking direction enemy moving and applying the relevant impulse in that direction.
	if(movingRight == true)
	{
		desiredVelocity = b2Min(enemyVelocity.x + 0.1f, 1.2f);
	}
	else if (movingRight == false)
	{
		desiredVelocity = b2Max(enemyVelocity.x - 0.1f, -1.2f);
	}
	else
	{
		//no direction, so halt this enemy object.
		desiredVelocity = enemyVelocity.x * -0.001f;
	}

	//get the required change, * by mass to get impulse and apply that impulse to the x-axis.
	velocityChange = desiredVelocity - enemyVelocity.x;
	impulse = body->GetMass() * velocityChange;
	body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0.0f), true);
}

//! Function to check current x-position to x position of range limits, turning direction of enemy object once those range limits are met.
/*!
\param - n/a
*/
void Enemy::rangeCheck()
{
	//grab current position of enemy on x-axis.
	float currentPosition = body->GetPosition().x;

	//check against range limits and change movingRight bool as required.
	if (currentPosition >= rightRange)
	{
		movingRight = false;
	}
	else if (currentPosition <= leftRange)
	{
		movingRight = true;
	}
}

//! Function to check if enemy object needs to change direction (told by objectContactListener) and doing so.
/*!
\param - n/a
*/
void Enemy::obstacleCollisionCheck()
{
	//check if changeDirection bool is true
	if (changeDirection == true)
	{
		//then check which direction enemy is currently moving in and reserve.
		if (movingRight == true)
		{
			movingRight = false;
		}
		else if (movingRight == false)
		{
			movingRight = true;
		}
	}
	//direction now changed, so reset changeDirection bool to false.
	changeDirection = false;
}