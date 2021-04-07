#pragma once
/*!
\file dynamicCircle.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physicalObject.h"
/*! \class DynamicCircle
\brief A circle which can move, rotate and collide with other objects.
*/

class DynamicCircle : public sf::CircleShape, public PhysicalObject {
public:
	DynamicCircle() {};		//!< default constructor.
	~DynamicCircle() {};	//!< default deconstructor.
	DynamicCircle(b2World * world, const sf::Vector2f& position, float radius, float orientation, uint16 categoryBits, uint16 maskBit, sf::Texture* texture);		//!< complete constructor.
	void update();	//!< update rendering info.
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;	//!< draw object.
};

