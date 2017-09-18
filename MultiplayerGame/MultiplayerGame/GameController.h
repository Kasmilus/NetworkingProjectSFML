#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D\Box2D.h>

#include "SFMLDebugDraw.h"
#include "Input.h"
#include "Timer.h"
#include "PhysicsObject.h"
#include "Player.h"


class GameController
{
public:
	GameController();
	~GameController();

	void Init(sf::RenderWindow *window);
	bool Update(float deltaTime);	// return false to quit game
	void Render();
	void CleanUp();

private:
	// Physics
	b2World* physicsWorld;
	SFMLDebugDraw* debugDraw;	// debug drawing
	const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const int VEL_ITERATIONS = 8;
	const int POS_ITERATIONS = 3;


	//Textures
	sf::Texture testTexture;

	//Objects
	PhysicsObject* testObj;
	PhysicsObject* wall;

	//SFML
	sf::RenderWindow *window;

};

