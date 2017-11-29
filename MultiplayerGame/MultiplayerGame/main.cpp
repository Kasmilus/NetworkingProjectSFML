#include <SFML/Graphics.hpp>

#include "GameController.h"
#include "Timer.h"
#include "Log.h"

const float MAP_SIZE = 100;

bool isWindowInFocus = true;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Multiplayer game");
	window.setVerticalSyncEnabled(false);	// enforce the same update rate on all clients and server
	window.setFramerateLimit(60);

	sf::View view(sf::FloatRect(-MAP_SIZE/2, MAP_SIZE, MAP_SIZE, -MAP_SIZE));
	window.setView(view);
	sf::Clock clockDeltaTime;	// This time is restarted each frame

	GameController game;
	game.Init(&window);

	LOG(INFO, INGAME) << "Started the game.";

	while (window.isOpen())
	{
		
		sf::Event event;
		//view.move(1, 1);
		sf::Time deltaTime = clockDeltaTime.restart();
		// Update game logic
		bool run = game.Update(deltaTime.asSeconds());

		// Rendering
		window.clear(sf::Color::Black);
		game.Render();
		window.display();

		while (window.pollEvent(event))
		{
			// Check if window is active
			if (event.type == sf::Event::GainedFocus) 
				isWindowInFocus = true;
			if (event.type == sf::Event::LostFocus) 
				isWindowInFocus = false;

			game.isWindowInFocus = isWindowInFocus;

			if (isWindowInFocus)
			{
				// "close requested" event: we close the window
				if (event.type == sf::Event::Closed || !run)
					window.close();
			}
		}
	}

	game.CleanUp();

	return 0;
}