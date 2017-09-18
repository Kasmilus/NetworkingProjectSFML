#include <SFML/Graphics.hpp>

#include "GameController.h"

const float MAP_SIZE = 100;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

	sf::View view(sf::FloatRect(-MAP_SIZE/2, MAP_SIZE, MAP_SIZE, -MAP_SIZE));
	window.setView(view);
	sf::Clock clock;

	GameController game;
	game.Init(&window);

	while (window.isOpen())
	{
		sf::Event event;
		//view.move(1, 1);
		sf::Time deltaTime = clock.restart();
		// Update game logic
		bool run = game.Update(deltaTime.asSeconds());

		// Rendering
		window.clear(sf::Color::Black);
		game.Render();
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