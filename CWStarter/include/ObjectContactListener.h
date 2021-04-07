#pragma once
/*!
\file ObjectContactListener.h
*/
#include <Box2D/Box2D.h>
#include "dynamicCircle.h"
#include "dynamicRect.h"
#include "staticRect.h"
#include "staticSensor.h"
#include "player.h"
#include "item.h"
#include "enemy.h"
#include "obstacle.h"
/*!
\class ObjectContactListener
\brief Listener, listening for contacts between objects in world and then implementing desired outcomes of those collisions.
*/

class ObjectContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact);	//!< function for entering a collision.
	void EndContact(b2Contact* contact);	//!< function for exiting a collision.

	void PreSolve(b2Contact* contact);		//!< function to implement actions pre ain contact of collision.
	void PostSolve(b2Contact* contact);		//!< function to implement actions post main contact of collision.

	void scoreCounter(int& totalScore);		//!< function to pass the playerScore when called.					
	void isPlayerGrounded(bool& canJump);	//!< function to pass whether player is on ground.
	void isPlayerDead(bool& isDead);		//!< function to pass whether player is dead.
	void playAudio(bool& coin, bool& enemy);	//!< function to pass whether certain audios need to be played.
};