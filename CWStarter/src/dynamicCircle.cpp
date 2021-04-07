#include "dynamicCircle.h"

/*! \file dynamicCircle.cpp
* \brief Contains functions to create and update a Dyanmic Circle object within the world.
*/


//! Function to create a dynamic circle object within the required world.
/*!
\param b2World world - the world to create this object in.
\param sf::Vector2f position - a Vec2f of the desired position co-ords in world.
\param sf::Vector2f position - a Vec2f of the desired size of object.
\param float orientation - a float for the required orientation of the object in world.
\param uint16 categoryBits - uint16 for physicalObject.h CollisionFilter enum, for what type object this is.
\param uint16 maskBits -  - uint16 for physicalObject.h CollisionFilter enum, for what types object this will collide with.
\param sf::Texture texture - an sfml reference to Texture to contain what texture this object should have.
*/
DynamicCircle::DynamicCircle(b2World * world, const sf::Vector2f& position, const float radius, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture)
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
	body->SetUserData(this); // used by our contact listener

	//the shape element of the object, circle so a radius.
	b2CircleShape shape;
	shape.m_radius = radius;

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
	setRadius(radius);
	setOrigin(radius, radius);
	setRotation(orientation);
}

//! Function to update the position and rotation of this object within its world.
/*!
\param - n/a
*/
void DynamicCircle::update()
{
	b2Vec2 position = body->GetPosition();
	setPosition(position.x, position.y);
	float angle = body->GetAngle() * RAD2DEG;
	setRotation(angle);
}

//! Function to draw a line on the circle object, makes it easier to see rotational effects.
/*!
\param sf::RenderTarget target - the target object required itself and line to be drawn on. 
\param sf::RenderStates states - second requirement of draw(), not currently used in this function.
*/
void DynamicCircle::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw((sf::CircleShape)*this); //draw the circle shape.

	//adding a line to the circle, so you can see the rotation.
	sf::RectangleShape line(sf::Vector2f(getRadius(), 0.01f));
	line.setPosition(getPosition());
	line.setOrigin(0.f, 0.005f);
	line.rotate(getRotation());
	target.draw(line);
}