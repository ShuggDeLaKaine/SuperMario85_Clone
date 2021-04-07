#include "staticSensor.h"

/*! \file staticSensor.cpp
* \brief Contains functions to create a static sensor within the world, to be used along side the ObjectCollisionListener.
*/

//! Function to create a Static Sensor 
/*!
\param b2World world - the world to create this object in.
\param sf::Vector2f position - a Vec2f of the desired position co-ords in world.
\param sf::Vector2f position - a Vec2f of the desired size of object.
\param float orientation - a float for the required orientation of the object in world.
*/
StaticSensor::StaticSensor(b2World* world, const sf::Vector2f& position, const sf::Vector2f& size, const float& orientation)
{
	//setting var for body defintion.
	b2BodyDef bodyDef;
	//setting position and angle of body.
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = orientation * DEG2RAD;
	//creating the body and putting into the world.
	body = world->CreateBody(&bodyDef);
	body->SetUserData(this);	// used by our contact listener

	//setting var for shape.
	b2PolygonShape shape;
	//setting size of rectangle, *0.5 to origin in centre, a rectangle so 0.0f radius.
	shape.SetAsBox(size.x * 0.5f, size.y * 0.5f);
	shape.m_radius = 0.0f;

	//setting var for fixture definition.
	b2FixtureDef fixtureDef;
	//defining the fixture properties of this sensor.
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.shape = &shape;

	//IMPORTANT - makes this object a sensor.
	fixtureDef.isSensor = true;
	//setting maskBits to 0 so it does not collide with anything.
	fixtureDef.filter.maskBits = 0;

	//attach fixture to the body of object.
	body->CreateFixture(&fixtureDef);
	
	//SFML - setting final position, size, origin, rotation, colour and outline.
	setPosition(position);
	setSize(size);
	setOrigin(size * 0.5f);
	setRotation(orientation);
	setFillColor(sf::Color::Yellow);
	setOutlineThickness(0.0f);
}

void StaticSensor::action()
{
	std::cout << "Sensor Hit" << std::endl;
}