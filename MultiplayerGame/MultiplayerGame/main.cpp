#include <SFML/Graphics.hpp>

#include "GameController.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
	sf::Clock clock;

	GameController game;
	game.Init();

	while (window.isOpen())
	{
		sf::Event event;

		sf::Time deltaTime = clock.restart();
		// Update game logic
		bool run = game.Update(deltaTime.asSeconds());

		// Rendering
		window.clear(sf::Color::Black);
		game.Render(window);
		window.display();

		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed || !run)
				window.close();
		}
	}

	game.CleanUp();

	return 0;
}