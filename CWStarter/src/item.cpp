/*! \file item.cpp
* \brief Contains function for creating an item object in the world.
* As well as functions to check if object requires removal and how to do so.
*/
#include "item.h"

//! Function to create an item object within the required world.
/*!
\param b2World world - the world to create this object in.
\param sf::Vector2f position - a Vec2f of the desired position co-ords in world.
\param sf::Vector2f position - a Vec2f of the desired size of object.
\param float orientation - a float for the required orientation of the object in world.
\param uint16 categoryBits - uint16 for physicalObject.h CollisionFilter enum, for what type object this is.
\param uint16 maskBits -  - uint16 for physicalObject.h CollisionFilter enum, for what types object this will collide with.
\param sf::Texture texture - an sfml reference to Texture to contain what texture this object should have.
*/
Item::Item(b2World* world, const sf::Vector2f position, const sf::Vector2f size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture)
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

	//initialising toRemove bool to false.
	toRemove = false;

	//initialising the object pool locations.
	b_objectPool = b2Vec2(-10.0f, -10.0f);
	sf_objectPool = sf::Vector2f(-10.0f, -10.0f);
}

//! Function to remove body, and therefore object, from world it is in.
/*!
/params - n/a
*/
Item::~Item()
{
	//get this body from world and destroy it from world.
	//body->GetWorld()->DestroyBody(body);
}

//! Function to check all that needs to be checked each frame/step. For now it's only whether the object needs to be removed.
/*!
*/
void Item::update()
{
	toBeRemovedCheck();
}

//! Function to to check whether toRemove bool is true and then deactive object and put back into its object pool location.
/*!
\param n/a
*/
void Item::toBeRemovedCheck()
{
	//if objectContactListener sets toRemove to true, then deactive body and move to object pool.
	if (toRemove == true)
	{
		body->SetActive(false);
		body->SetTransform(b2Vec2(b_objectPool), 0.0f);
		setPosition(sf_objectPool);
	}
}