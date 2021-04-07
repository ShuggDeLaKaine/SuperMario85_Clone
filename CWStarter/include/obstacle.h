#pragma once
/*!
\file obstacle.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physicalObject.h"

/*! \class Obstacle
\brief A simple in world obstacle made from static rectangle objects that can collide with other objects.
*/
class Obstacle : public sf::RectangleShape, public PhysicalObject {
public:
	Obstacle() {};	//!< default constructor.
	~Obstacle() {};	//!< default deconstructor.
	Obstacle(b2World* world, const sf::Vector2f& position, const sf::Vector2f& size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture);		//!< complete constructor.
};