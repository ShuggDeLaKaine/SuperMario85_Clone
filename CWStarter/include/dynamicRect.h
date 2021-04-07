#pragma once
/*!
\file dynamicRect.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physicalObject.h"
/*! \class DynamicBlock
\brief A simple rectangle object which can move, rotate and collide with stuff
*/
class DynamicRect : public sf::RectangleShape, public PhysicalObject {
public:
	DynamicRect() {};	//!< default constructor.
	~DynamicRect() {};	//!< default deconstructor.
	DynamicRect(b2World* world, const sf::Vector2f position, const sf::Vector2f size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture);		//!< complete constructor.
	void update();		//!< function to update the rendering information for these objects.
};