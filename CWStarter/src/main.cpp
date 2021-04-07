/*! \file main.cpp
\brief Contains the entry point for the application.
*/

/*! \mainpage 1985 Super Mario Bros Clone
* This is a clone of the 1985 Super Mario Bros game for the Nintendo.
* It contains the first level of the game, to complete you must reach the flagpole at the finish.
*/

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "game.h"

void main() /** Entry point for the application */
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "1985 Super Mario Bros Clone"); // Open main window

	//reference to the game class.
	Game game;

	//make a lovely blue sky colour
	sf::Color lovelyMarioBlue(107, 140, 255);

	//setting the fps to 60 per second.
	float frameTime = 1.0f / 60.f;

	// Start a clock to provide time for the physics
	sf::Clock clock;

	// Run a game loop
	while (window.isOpen())
	{
	   	sf::Event event;

		while (window.pollEvent(event))
		{
			// window closed button pressed
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			//event if key pressed, access the userInput() in game.cpp
			if (event.type == sf::Event::KeyPressed)
			{
				game.userInput(event.key.code);
			}
			//event if key released, access the stopMovement() in game.cpp
			else if (event.type == sf::Event::KeyReleased)
			{
				game.stopMovement(event.key.code);
			}
		}

		// Find out how much time has elapsed
		float elapsedTime = clock.getElapsedTime().asSeconds();

		// If a frame has past the update the physics
		if (elapsedTime > frameTime)
		{
			game.update(elapsedTime);
			clock.restart();
		}

		//clear background to required colour.
		window.clear(lovelyMarioBlue);
		//draw the game.
		window.draw(game);
		//display to window the game.
		window.display();
	}

}





