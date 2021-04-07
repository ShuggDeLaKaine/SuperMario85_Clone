#pragma once
/*!
\file staticSensor.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "physicalObject.h"
/*! \class StaticSensor
\brief A block with no movement, doesn't collide with anything else. Used by the contact listener.
*/

class StaticSensor : public sf::RectangleShape, public PhysicalObject
{
public:
	StaticSensor() {}; //!< Default Constructor.
	~StaticSensor() {}; //!< Default Deconstructor.
	StaticSensor(b2World* world, const sf::Vector2f& position, const sf::Vector2f& size, const float& orientation); //!< complete constructor.
	void action(); //!< function for sensor, used for on contact to say there's been a collision.
};