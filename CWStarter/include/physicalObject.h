#pragma once
/*!
\file physicalObject.h
*/
#include <Box2D/Box2D.h>
#include <iostream>
#define DEG2RAD 0.017453f
#define RAD2DEG 57.29577f
/*!
\class ObjectObject
\brief Simple class with common attributes for physical objects in the world, including density, friction, resitution.
\ As well as common functions such as getBody() and setUserDate().
*/

class PhysicalObject {
protected:
	b2Body* body = nullptr;		//!< Box2D body
	float density = 1.0f;		//!< density of objects, its mass.
	float friction = 0.0f;		//!< friction of objects whilst moving.
	float restitution = 0.6f;	//!< the 'bouncyness' of objects.
public:
	enum CollisionFilter {
		PLAYER = 1,
		ENEMY = 2,
		GROUND = 4,
		BOUNDARY = 8,
		BREAKABLE_BOX = 16,
		UNBREAKABLE_BOX = 32,
		CHEST_BOX = 64,
		ITEM = 128,
		OBSTACLE = 256,
		MOVING_OBJECT = 516
	};	//!< enums for collision filtering, principle types of objects that will be within the world.

	void setUserData(void* data) { body->SetUserData(data); }	//!< function to set the user data of a body.
	b2Body* getBody() const { return body; }	//!< function to return the body element of an object.

	//! Function to load a texture.
	/*!
	\param std::string fileName - string which contains the file name of the texture to be loaded.
	\param sf::Texture texture - the required texture to be loaded.
	\return bool - whether load of texture was successful or not. 
	*/
	virtual bool loadTexture(sf::Texture& texture, std::string fileName)
	{
		bool success;
		//check whether load attempt was successful, if not an error to console and exit program.
		success = texture.loadFromFile("./assets/textures/" + fileName);
		if (!success)
		{
			std::cout << "texture for " + fileName + " not loaded" << std::endl;
			return false;
		}
		return true;
	} //!< func to be used to load a texture to the physical objects; dynamic circles, static rectangle etc.
};