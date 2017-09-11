#pragma once

#include <SFML/Graphics.hpp>
#include "Input.h"
#include "Timer.h"
#include <Box2D\Box2D.h>

class GameController
{
public:
	GameController();
	~GameController();

	void Init();
	bool Update(float deltaTime);	// return false to quit game
	void Render(sf::RenderWindow &window);
	void CleanUp();

private:


	// Test
	sf::Texture testTexture;
	sf::Sprite testSprite;

};

