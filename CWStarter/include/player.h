#pragma once
/*!
\file player.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physicalObject.h"
/*! \class Player
\brief Class to create the player object.
*/

class Player : public sf::RectangleShape, public PhysicalObject
{
private:
	sf::Sprite marioSprite;		//!< sprite to take required sprite for mario.
	int numFrames;		//!< int to take number of frames within the spritesheet.
	float animDuration;	//!< float for the time length for each frame in the animation.
public:
	Player() {};		//!< default constructor.
	~Player() {};		//!< default deconstructor.
	Player(b2World* world, const sf::Vector2f position, const sf::Vector2f size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture, sf::Sprite* sprite, int frames, float animDur); //!< complete constructor.
	void update();		//!< function to update the rendering information for these objects.	
};