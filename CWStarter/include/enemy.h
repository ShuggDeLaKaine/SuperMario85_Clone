#pragma once
/*!
\file enemy.h 
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physicalObject.h"
/*! \class Player
\brief Class to hold the player create enemy object.
*/

class Enemy : public sf::RectangleShape, public PhysicalObject {
private:
	sf::Sprite enemySprite;		//!< sprite to take required sprite for mario.
	int numFrames;				//!< int to take number of frames within the spritesheet.
	float animDuration;			//!< float for the time length for each frame in the animation.

	b2Vec2 b_objectPool;		//!< box2d vector2 for objectPool location, for moving the body of object.
	sf::Vector2f sf_objectPool;	//!< sfml vector2 for objectPool location, for moving the sprite/texture of object.

	b2Vec2 enemyVelocity;	//!< to hold the current velocity of enmey.
	float desiredVelocity;	//!< the desired velocity.
	float velocityChange;	//!< change of velocity required.
	float impulse;			//!< strength of impulse to be applied as a force on a body.

	bool movingRight;		//!< bool to determine whether enemy object should be moving left or right.
	float movementRange;	//!< float to contain size of movement range of enemy object.
	float rightRange;		//!< float to contain right max range.
	float leftRange;		//!< float to contain left max range.

	void enemyMovement();	//!< function to move the enemy object within scene.
	void rangeCheck();		//!< function to check that enemy object is still within its set range & change direction when limits reached.
	void toBeRemovedCheck();//!< function to check whether this object needs to be removed from the scene.
	void obstacleCollisionCheck();	//!< function to check whether enemy object has collided with a obstacle.
public:
	Enemy() {}				//!< default constructor.
	~Enemy();				//!< default deconstructor.
	Enemy(b2World* world, const sf::Vector2f& position, const sf::Vector2f size, float orientation, uint16 cateogoryBits, uint16 maskBits, sf::Texture* texture, sf::Sprite* sprite, int frames, float animDur); //!< complete constructor.

	void update();			//!< update rendering information.
	bool toRemove;			//!< bool to determine whether this object needs to be added to a removal list.
	bool changeDirection;	//!< bool to determine whether enemy object has collided with an obstacle and needs to change direction.
};