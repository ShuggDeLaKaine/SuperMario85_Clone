#include "dynamicRect.h"

/*! \file dynamicRect.cpp
* \brief Contains functions to create and update a Dyanmic Rectangle object within the world.
*/

//! Function to create a dynamic rectangle object within the required world.
/*!
\param b2World world - the world to create this object in.
\param sf::Vector2f position - a Vec2f of the desired position co-ords in world.
\param sf::Vector2f position - a Vec2f of the desired size of object.
\param float orientation - a float for the required orientation of the object in world.
\param uint16 categoryBits - uint16 for physicalObject.h CollisionFilter enum, for what type object this is.
\param uint16 maskBits -  - uint16 for physicalObject.h CollisionFilter enum, for what types object this will collide with.
\param sf::Texture texture - an sfml reference to Texture to contain what texture this object should have.
*/
DynamicRect::DynamicRect(b2World* world, const sf::Vector2f position, const sf::Vector2f size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture)
{
	//setting the texture for the object from the texture provided in the parameters.
	setTexture(texture);

	//the body element of object, setting its position, orientation and type.
	b2BodyDef bodyDef;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = orientation * DEG2RAD;
	bodyDef.type = b2_dynamicBody;

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

	//attaching a 'foot' sensor to object.
	shape.SetAsBox(0.45f, 0.25f, b2Vec2(0.0f, -size.y), 0.0f);
	fixtureDef.isSensor = true;
	b2Fixture* footSensorFixture = body->CreateFixture(&fixtureDef);
	footSensorFixture->SetUserData((void*)2); //!< setting the user date tag of a 3 for the foot of Mario.

	//using some SFML functions to set the position of the object within the scene.
	setPosition(position);
	setSize(size);
	setOrigin(size * 0.5f);
	setRotation(orientation);
	setOutlineThickness(0.0f);
}

//! Function to update the position and rotation of this object within its world.
/*!
\param - n/a
*/
void DynamicRect::update()
{
	//updating the position and angle of this object.
	b2Vec2 position = body->GetPosition();
	setPosition(position.x, position.y);
	float angle = body->GetAngle() * RAD2DEG;
	setRotation(angle);
}