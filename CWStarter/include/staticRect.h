#pragma once
/*!
\file staticRect.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physicalObject.h"
/*! \class StaticRect
\brief A simple static rectangle object that can collide with other objects.
*/

class StaticRect : public sf::RectangleShape, public PhysicalObject {
public:
	StaticRect() {};	//!< default constructor.
	~StaticRect() {};	//!< default deconstructor.
	StaticRect(b2World* world, const sf::Vector2f& position, const sf::Vector2f& size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture);	//!< complete constructor 
};