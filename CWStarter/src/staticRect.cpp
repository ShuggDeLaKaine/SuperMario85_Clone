#include "staticRect.h"
/*! \file staticRect.cpp
* \brief Contains function for creating a static rectangle object in game.
*/

//! Function to create a static Rectangle object within the required world.
/*!
\param b2World world - the world to create this object in.
\param sf::Vector2f position - a Vec2f of the desired position co-ords in world.
\param sf::Vector2f position - a Vec2f of the desired size of object.
\param float orientation - a float for the required orientation of the object in world.
\param uint16 categoryBits - uint16 for physicalObject.h CollisionFilter enum, for what type object this is.
\param uint16 maskBits -  - uint16 for physicalObject.h CollisionFilter enum, for what types object this will collide with.
\param sf::Texture texture - an sfml reference to Texture to contain what texture this object should have.
*/
StaticRect::StaticRect(b2World* world, const sf::Vector2f& position, const sf::Vector2f& size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture)
{
	//setting the texture for the object from the texture provided in the parameters.
	setTexture(texture);

	//the body element of object, setting its position and orientation.
	b2BodyDef bodyDef;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = orientation * DEG2RAD;
	//creating a body in the world and assigning the above attributes.
	body = world->CreateBody(&bodyDef);
	body->SetUserData(this);	//to be used by the contact listener.

	//the shape element of the object, as it's a rectangle no radius.
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
}