#pragma once
/*!
\file item.h
*/
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <set>
#include <vector>
#include "physicalObject.h"
#include "staticRect.h"
/*! \class Item
\brief A class for items which can collide only with the player.
*/

class Item : public StaticRect
{
private:
	void toBeRemovedCheck();	//!< function to check whether this object needs to be removed from the scene.
	b2Vec2 b_objectPool;		//!< box2d vector2 for objectPool location, for moving the body of object.
	sf::Vector2f sf_objectPool;	//!< sfml vector2 for objectPool location, for moving the sprite/texture of object.
public:
	Item() {}			//!< default constructor
	~Item();			//!< deafult deconstructor
	Item(b2World* world, const sf::Vector2f position, const sf::Vector2f size, const float orientation, uint16 categoryBits, uint16 maskBits, sf::Texture* texture); //!< complete constructor.
	void update();		//!< function to update the rendering information for these objects.	
	bool toRemove;		//!< bool to determine whether this object needs to be added to a removal list.
};