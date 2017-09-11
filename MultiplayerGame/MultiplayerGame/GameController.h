#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D\Box2D.h>

#include "Input.h"
#include "Timer.h"
#include "PhysicsObject.h"


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
	// Physics
	b2World* physicsWorld;
	const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const int VEL_ITERATIONS = 8;
	const int POS_ITERATIONS = 3;


	PhysicsObject* testObj;
	PhysicsObject* wall;

};

