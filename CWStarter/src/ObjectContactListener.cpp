#include "ObjectContactListener.h"

/*!\file ObjectContactListener.cpp
* \brief Contains all required functions for Beginning and Ending collisions.
* As well as for Pre and Post solve functions.
* Also function that pass values onto the game.cpp for in-game decision making.
*/

//global int initialised to 0.
int playerScore{};
//! Function to pass int playerScore to other class.
/*!
\param int totalScore - int that will take the passed int value of playerScore.
*/
void ObjectContactListener::scoreCounter(int& totalScore)
{
	totalScore = playerScore;
}

//global bool initialised to false.
bool isGrounded{};
//! Function to pass bool isGrounded to other class.
/*!
\param bool canJump - bool to take the value of isGrounded.
*/
void ObjectContactListener::isPlayerGrounded(bool& canJump)
{
	canJump = isGrounded;
}

//global bool initialised to false.
bool playerDead{};
//! Function to pass bool playerDead to other class.
/*!
\param bool isDead - bool to take the value of playerDead.
*/
void ObjectContactListener::isPlayerDead(bool& isDead)
{
	isDead = playerDead;
}

//global bools initialised to false.
bool coinCollectSFX{};
bool enemyHurtSFX{};
//! Function to pass bools coinCollectSFX & enemyHurtSFX to other class for whether the relevant sound FX should be played.
/*!
\param bool coin - bool to take the value of coinColletSFX.
\param bool enemy - bool to take the value of enemyHurtSFX.
*/
void ObjectContactListener::playAudio(bool& coin, bool& enemy)
{
	coin = coinCollectSFX;
	enemy = enemyHurtSFX;

	//reset bools after use.
	if (enemyHurtSFX == true)
	{
		enemyHurtSFX = false;
	}
	if (coinCollectSFX == true)
	{
		coinCollectSFX = false;
	}
}

//! Function to be called on two object entering a collision.
/*!
\param b2Contact contact - b2Contact class for overlapping AABB of two objects set to collide in collision filters.
*/
void ObjectContactListener::BeginContact(b2Contact* contact)
{
	//setting the bodies of each contact object.
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	//using RTTI here to set userData to a new pair, maps a string to a void.
	//string is the RTTI type name and void* will point to runtime variable.
	std::pair<std::string, void *> dataA = *(std::pair<std::string, void *>*) bodyA->GetUserData();
	std::pair<std::string, void *> dataB = *(std::pair<std::string, void *>*) bodyB->GetUserData();

	//bools to check whether either contact objects are sensors.
	bool isSensorA = contact->GetFixtureA()->IsSensor();
	bool isSensorB = contact->GetFixtureB()->IsSensor();

	//checking whether either is sensor and calling action function if so.
	if (isSensorA == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyA->GetUserData());
		sensor->action();
		return;
	}
	if (isSensorB == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyB->GetUserData());
		sensor->action();
		return;
	}

	//PLAYER object ENTER collision with a GROUND object.
	if (typeid(Player).name() == dataA.first)
	{
		if (typeid(StaticRect).name() == dataB.first)
		{
			Player *player = static_cast<Player*>(dataA.second);
			StaticRect *ground = static_cast<StaticRect*>(dataB.second);

			//get y-axis positions of both player and enemy objects.
			float playerYPos = player->getBody()->GetPosition().y;
			float groundYPos = ground->getBody()->GetPosition().y;

			//get the difference between the two positions.
			float posDiff = ((playerYPos - groundYPos) * -1.0f);

			//getting size of enemy object and a 50% of that as an offset.
			float groundSize = ground->getGlobalBounds().height;
			float sizeOffset = groundSize * 0.50f;

			//check if posDiff bigger than groundSize & offset
			//if so then has landed on the ground from above, so is grounded.
			if (posDiff > (groundSize - sizeOffset))
			{
				if (player != nullptr)
				{
					//collided with ground, so isGrounded is now true.
					isGrounded = true;
					//need to take the impulse out of the y.axis on landing to prevent bouncying.
					float xImpulse = player->getBody()->GetLinearVelocity().x;
					player->getBody()->SetLinearVelocity(b2Vec2(xImpulse, 0.0f));
				}
			}
			//otherwise hit ground object from the side or below.
			else
			{
				if (player != nullptr)
				{
				}
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Player).name() == dataB.first)
	{
		if (typeid(StaticRect).name() == dataA.first)
		{
			Player *player = static_cast<Player*>(dataB.second);
			StaticRect *ground = static_cast<StaticRect*>(dataA.second);

			//get y-axis positions of both player and enemy objects.
			float playerYPos = player->getBody()->GetPosition().y;
			float groundYPos = ground->getBody()->GetPosition().y;

			//get the difference between the two positions.
			float posDiff = ((playerYPos - groundYPos) * -1.0f);

			//getting size of enemy object and a 50% of that as an offset.
			float groundSize = ground->getGlobalBounds().height;
			float sizeOffset = groundSize * 0.50f;

			//check if posDiff bigger than groundSize & offset
			//if so then has landed on the ground from above, so is grounded.
			if (posDiff > (groundSize - sizeOffset))
			{
				if (player != nullptr)
				{
					//collided with ground, so isGrounded is now true.
					isGrounded = true;
					//need to take the impulse out of the y.axis on landing to prevent bouncying.
					float xImpulse = player->getBody()->GetLinearVelocity().x;
					player->getBody()->SetLinearVelocity(b2Vec2(xImpulse, 0.0f));
				}
			}
			//otherwise hit ground object from the side or below.
			else
			{
				if (player != nullptr)
				{
				}
			}
		}
	}

	//ENEMY object ENTERS collision with OBSTACLE.
	if (typeid(Enemy).name() == dataA.first)
	{
		if (typeid(Obstacle).name() == dataB.first)
		{
			Enemy *enemy = static_cast<Enemy*>(dataA.second);
			if (enemy != nullptr)
			{
				//flip bool to say direction change required.
				enemy->changeDirection = true;
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Enemy).name() == dataB.first)
	{
		if (typeid(Obstacle).name() == dataA.first)
		{
			Enemy *enemy = static_cast<Enemy*>(dataB.second);
			if (enemy != nullptr)
			{
				//flip bool to say direction change required.
				enemy->changeDirection = true;
			}
		}
	}

	//ENEMY object ENTERS collision with another ENEMY.
	if (typeid(Enemy).name() == dataA.first)
	{
		if (typeid(Enemy).name() == dataB.first)
		{
			Enemy *enemy1 = static_cast<Enemy*>(dataA.second);
			Enemy *enemy2 = static_cast<Enemy*>(dataB.second);
			if (enemy1 != nullptr)
			{
				//flip bool to say direction change required.
				enemy1->changeDirection = true;
			}
			if (enemy2 != nullptr)
			{
				//flip bool to say direction change required.
				enemy2->changeDirection = true;
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Enemy).name() == dataB.first)
	{
		if (typeid(Enemy).name() == dataA.first)
		{
			Enemy *enemy1 = static_cast<Enemy*>(dataB.second);
			Enemy *enemy2 = static_cast<Enemy*>(dataA.second);
			if (enemy1 != nullptr)
			{
				//flip bool to say direction change required.
				enemy1->changeDirection = true;
			}
			if (enemy2 != nullptr)
			{
				//flip bool to say direction change required.
				enemy2->changeDirection = true;
			}
		}
	}

	//collision ENTER between PLAYER and ENEMY object.
	if (typeid(Player).name() == dataA.first)
	{
		if (typeid(Enemy).name() == dataB.first)
		{
			Player *player = static_cast<Player*>(dataA.second);
			Enemy *enemy = static_cast<Enemy*>(dataB.second);

			//get y-axis positions of both player and enemy objects.
			float playerYPos = player->getBody()->GetPosition().y;
			float enemyYPos = enemy->getBody()->GetPosition().y;

			//get the difference between the two positions.
			float posDiff = ((playerYPos - enemyYPos) * -1.0f);

			//getting size of enemy object and a 20% of that as an offset.
			float enemySize = enemy->getGlobalBounds().height;
			float sizeOffset = enemySize * 0.20f;

			//check if posDiff bigger than enemySize & offset
			//if so then has landed on the enemy from above, so killed it.
			if (posDiff > (enemySize - sizeOffset))
			{
				if(enemy != nullptr)
				{
					enemy->toRemove = true;
					enemyHurtSFX = true;
					playerScore = playerScore + 100;
				}
			}
			//otherwise the enemy has hit and killed the player.
			else
			{
				if (player != nullptr)
				{
					playerDead = true;
				}
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Player).name() == dataB.first)
	{
		if (typeid(Enemy).name() == dataA.first)
		{
			Player *player = static_cast<Player*>(dataB.second);
			Enemy *enemy = static_cast<Enemy*>(dataA.second);

			//get y-axis positions of both player and enemy objects.
			float playerYPos = player->getBody()->GetPosition().y;
			float enemyYPos = enemy->getBody()->GetPosition().y;

			//get the difference between the two positions.
			float posDiff = ((playerYPos - enemyYPos) * -1.0f);

			//getting size of enemy object and a 20% of that as an offset.
			float enemySize = enemy->getGlobalBounds().height;
			float sizeOffset = enemySize * 0.20f;

			//check if posDiff bigger than enemySize & offset
			//if so then has landed on the enemy from above, so killed it.
			if (posDiff > (enemySize - sizeOffset))
			{
				if (enemy != nullptr)
				{
					enemy->toRemove = true;
					enemyHurtSFX = true;
					playerScore = playerScore + 100;
				}
			}
			//otherwise the enemy has hit and killed the player.
			else
			{
				if (player != nullptr)
				{
					playerDead = true;
				}
			}
		}
	}

	//ITEM ENTERS collision with PLAYER.
	if (typeid(Item).name() == dataA.first)
	{
		if (typeid(Player).name() == dataB.first)
		{
			Item *item = static_cast<Item*>(dataA.second);
			Player *player = static_cast<Player*>(dataB.second);

			if (item != nullptr)
			{
				//stop forces being applied to player on contact with item.
				player->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				//increase player score.
				playerScore = playerScore + 10;
				//set player sound for item collection to true.
				coinCollectSFX = true;
				//change item.cpp bool to true.
				item->toRemove = true;
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Item).name() == dataB.first)
	{
		if (typeid(Player).name() == dataA.first)
		{
			Item *item = static_cast<Item*>(dataB.second);
			Player *player = static_cast<Player*>(dataA.second);

			if (item != nullptr)
			{
				//stop forces being applied to player on contact with item.
				player->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				//increase player score.
				playerScore = playerScore + 10;
				//set player sound for item collection to true.
				coinCollectSFX = true;
				//change item.cpp bool to true.
				item->toRemove = true;
			}
		}
	}	

	//PLAYER object ENTERS collision with a OBSTACLE object.
	if (typeid(Player).name() == dataA.first)
	{
		if (typeid(Obstacle).name() == dataB.first)
		{
			Player *player = static_cast<Player*>(dataA.second);
			Obstacle *obstacle = static_cast<Obstacle*>(dataB.second);

			if (player != nullptr)
			{
				//get y-axis positions of both player and enemy objects.
				float playerYPos = player->getBody()->GetPosition().y;
				float obstacleYPos = obstacle->getBody()->GetPosition().y;

				//get the difference between the two positions.
				float posDiff = ((playerYPos - obstacleYPos) * -1.0f);

				//getting size of enemy object and a 40% of that as an offset.
				float obstacleSize = obstacle->getGlobalBounds().height;
				float sizeOffset = obstacleSize * 0.40f;

				//check if posDiff bigger than obstacleSize & offset
				//if so then has landed on top of obstacle, take bounce force out and set to grounded.
				if (posDiff > (obstacleSize - sizeOffset))
				{
					if (obstacle != nullptr)
					{
						//collided with ground, so isGrounded is now true.
						isGrounded = true;
						//need to take the impulse out of the y.axis on landing to prevent bouncying.
						float xImpulse = player->getBody()->GetLinearVelocity().x;
						player->getBody()->SetLinearVelocity(b2Vec2(xImpulse, 0.0f));
					}
				}
				//otherwise hit the side, take impulse out of x-axis.
				else
				{
					if (obstacle != nullptr)
					{
						float yImpulse = player->getBody()->GetLinearVelocity().y;
						player->getBody()->SetLinearVelocity(b2Vec2(0.0f, yImpulse));
					}
				}
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Player).name() == dataB.first)
	{
		if (typeid(Obstacle).name() == dataA.first)
		{
			Player *player = static_cast<Player*>(dataB.second);
			Obstacle *obstacle = static_cast<Obstacle*>(dataA.second);

			if (player != nullptr)
			{
				//get y-axis positions of both player and enemy objects.
				float playerYPos = player->getBody()->GetPosition().y;
				float obstacleYPos = obstacle->getBody()->GetPosition().y;

				//get the difference between the two positions.
				float posDiff = ((playerYPos - obstacleYPos) * -1.0f);

				//getting size of enemy object and a 40% of that as an offset.
				float obstacleSize = obstacle->getGlobalBounds().height;
				float sizeOffset = obstacleSize * 0.40f;

				//check if posDiff bigger than obstacleSize & offset
				//if so then has landed on top of obstacle, take bounce force out and set to grounded.
				if (posDiff > (obstacleSize - sizeOffset))
				{
					if (obstacle != nullptr)
					{
						//collided with ground, so isGrounded is now true.
						isGrounded = true;
						//need to take the impulse out of the y.axis on landing to prevent bouncying.
						float xImpulse = player->getBody()->GetLinearVelocity().x;
						player->getBody()->SetLinearVelocity(b2Vec2(xImpulse, 0.0f));
					}
				}
				//otherwise hit the side, take impulse out of x-axis.
				else
				{
					if (obstacle != nullptr)
					{
						float yImpulse = player->getBody()->GetLinearVelocity().y;
						player->getBody()->SetLinearVelocity(b2Vec2(0.0f, yImpulse));
					}
				}
			}
		}
	}
}

//! Function to be called on the exit of two objects colliding.
/*!
param b2Contact contact - b2Contact class for overlapping AABB of two objects set to collide in collision filters.
*/
void ObjectContactListener::EndContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	std::pair<std::string, void *> dataA = *(std::pair<std::string, void *>*) bodyA->GetUserData();
	std::pair<std::string, void *> dataB = *(std::pair<std::string, void *>*) bodyB->GetUserData();

	//bools to check whether either contact objects are sensors.
	bool isSensorA = contact->GetFixtureA()->IsSensor();
	bool isSensorB = contact->GetFixtureB()->IsSensor();

	//checking whether either is sensor and calling action function if so.
	if (isSensorA == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyA->GetUserData());
		sensor->action();
		return;
	}
	if (isSensorB == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyB->GetUserData());
		sensor->action();
		return;
	}

	//PLAYER object EXITS collision with a GROUND object.
	if (typeid(Player).name() == dataA.first)
	{
		if (typeid(StaticRect).name() == dataB.first)
		{

			Player *player = static_cast<Player*>(dataA.second);
			StaticRect *ground = static_cast<StaticRect*>(dataB.second);

			//get y-axis positions of both player and enemy objects.
			float playerYPos = player->getBody()->GetPosition().y;
			float groundYPos = ground->getBody()->GetPosition().y;

			//get the difference between the two positions.
			float posDiff = ((playerYPos - groundYPos) * -1.0f);

			//getting size of enemy object and a 40% of that as an offset.
			float groundSize = ground->getGlobalBounds().height;
			float sizeOffset = groundSize * 0.40f;

			//check if posDiff bigger than groundSize & offset
			//if so then has landed on the ground from above, so is grounded.
			if (posDiff > (groundSize - sizeOffset))
			{
				if (player != nullptr)
				{
					//exited contact with ground, so no longer isGrounded.
					isGrounded = false;
				}
			}
			else
			{
				if (player != nullptr)
				{
				}
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Player).name() == dataB.first)
	{
		if (typeid(StaticRect).name() == dataA.first)
		{

			Player *player = static_cast<Player*>(dataB.second);
			StaticRect *ground = static_cast<StaticRect*>(dataA.second);

			//get y-axis positions of both player and enemy objects.
			float playerYPos = player->getBody()->GetPosition().y;
			float groundYPos = ground->getBody()->GetPosition().y;

			//get the difference between the two positions.
			float posDiff = ((playerYPos - groundYPos) * -1.0f);

			//getting size of enemy object and a 40% of that as an offset.
			float groundSize = ground->getGlobalBounds().height;
			float sizeOffset = groundSize * 0.40f;

			//check if posDiff bigger than groundSize & offset
			//if so then has landed on the ground from above, so is grounded.
			if (posDiff > (groundSize - sizeOffset))
			{
				if (player != nullptr)
				{
					//exited contact with ground, so no longer isGrounded.
					isGrounded = false;
				}
			}
			//otherwise hit ground from side.
			else
			{
				if (player != nullptr)
				{
				}
			}
		}
	}


	//collision EXIT between PLAYER and ENEMY object.
	if (typeid(Player).name() == dataA.first)
	{
		if (typeid(Enemy).name() == dataB.first)
		{
			Player *player = static_cast<Player*>(dataA.second);
			Enemy *enemy = static_cast<Enemy*>(dataB.second);
			if (player != nullptr) 
			{
				playerDead = false;
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Player).name() == dataB.first)
	{
		if (typeid(Enemy).name() == dataA.first)
		{
			Player *player = static_cast<Player*>(dataB.second);
			Enemy *enemy = static_cast<Enemy*>(dataA.second);
			if (player != nullptr)
			{
				playerDead = false;
			}
		}
	}

	//ITEM EXITS collision with PLAYER.
	if (typeid(Item).name() == dataA.first)
	{
		if (typeid(Player).name() == dataB.first)
		{
			Item *item = static_cast<Item*>(dataA.second);
			if (item != nullptr)
			{
				//
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Item).name() == dataB.first)
	{
		if (typeid(Player).name() == dataA.first)
		{
			Item *item = static_cast<Item*>(dataB.second);
			if (item != nullptr)
			{
				//
			}
		}
	}

	//PLAYER object EXITS collision with a OBSTACLE object.
	if (typeid(Player).name() == dataA.first)
	{
		if (typeid(Obstacle).name() == dataB.first)
		{
			Player *player = static_cast<Player*>(dataA.second);
			Obstacle *obstacle = static_cast<Obstacle*>(dataB.second);

			if (player != nullptr)
			{
				//get y-axis positions of both player and enemy objects.
				float playerYPos = player->getBody()->GetPosition().y;
				float obstacleYPos = obstacle->getBody()->GetPosition().y;

				//get the difference between the two positions.
				float posDiff = ((playerYPos - obstacleYPos) * -1.0f);

				//getting size of enemy object and a 40% of that as an offset.
				float obstacleSize = obstacle->getGlobalBounds().height;
				float sizeOffset = obstacleSize * 0.40f;

				//check if posDiff bigger than obstacle size & offset
				//so on top of obstacle and jumping off.
				if (posDiff > (obstacleSize - sizeOffset))
				{
					if (obstacle != nullptr)
					{
						//jumping off top of obstacle
						isGrounded = false;
					}
				}
				else
				{
					if (obstacle != nullptr)
					{
					}
				}
			}
		}
	}
	//the B-A-B of the above.
	if (typeid(Player).name() == dataB.first)
	{
		if (typeid(Obstacle).name() == dataA.first)
		{
			Player *player = static_cast<Player*>(dataB.second);
			Obstacle *obstacle = static_cast<Obstacle*>(dataA.second);

			if (player != nullptr)
			{
				//get y-axis positions of both player and enemy objects.
				float playerYPos = player->getBody()->GetPosition().y;
				float obstacleYPos = obstacle->getBody()->GetPosition().y;

				//get the difference between the two positions.
				float posDiff = ((playerYPos - obstacleYPos) * -1.0f);

				//getting size of enemy object and a 40% of that as an offset.
				float obstacleSize = obstacle->getGlobalBounds().height;
				float sizeOffset = obstacleSize * 0.40f;

				//check if posDiff bigger than obstacle size & offset
				//so on top of obstacle and jumping off.
				if (posDiff > (obstacleSize - sizeOffset))
				{
					if (obstacle != nullptr)
					{
						//jumping off top of obstacle
						isGrounded = false;
					}
				}
				else
				{
					if (obstacle != nullptr)
					{
					}
				}
			}
		}
	}
}

void ObjectContactListener::PreSolve(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	std::pair<std::string, void *> dataA = *(std::pair<std::string, void *>*) bodyA->GetUserData();
	std::pair<std::string, void *> dataB = *(std::pair<std::string, void *>*) bodyB->GetUserData();

	//bools to check whether either contact objects are sensors.
	bool isSensorA = contact->GetFixtureA()->IsSensor();
	bool isSensorB = contact->GetFixtureB()->IsSensor();

	//checking whether either is sensor and calling action function if so.
	if (isSensorA == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyA->GetUserData());
		sensor->action();
		return;
	}
	if (isSensorB == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyB->GetUserData());
		sensor->action();
		return;
	}

}

void ObjectContactListener::PostSolve(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	std::pair<std::string, void *> dataA = *(std::pair<std::string, void *>*) bodyA->GetUserData();
	std::pair<std::string, void *> dataB = *(std::pair<std::string, void *>*) bodyB->GetUserData();

	//bools to check whether either contact objects are sensors.
	bool isSensorA = contact->GetFixtureA()->IsSensor();
	bool isSensorB = contact->GetFixtureB()->IsSensor();

	//checking whether either is sensor and calling action function if so.
	if (isSensorA == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyA->GetUserData());
		sensor->action();
		return;
	}
	if (isSensorB == true)
	{
		StaticSensor *sensor = static_cast <StaticSensor *> (bodyB->GetUserData());
		sensor->action();
		return;
	}

}