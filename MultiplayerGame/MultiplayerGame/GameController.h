#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D\Box2D.h>

#include "SFMLDebugDraw.h"
#include "Input.h"
#include "Timer.h"
#include "PhysicsObject.h"
#include "Player.h"
#include "ContactListener.h"
#include <vector>

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
	void SpawnObjects();	// Creates level

private:
	// Physics
	b2World* physicsWorld;
	ContactListener* contactListener;
	SFMLDebugDraw* debugDraw;	// debug drawing
	const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const int VEL_ITERATIONS = 8;
	const int POS_ITERATIONS = 3;

	//Textures
	sf::Texture testTexture;
	sf::Texture playerTexture;
	sf::Texture wallTexture;
	sf::Texture crateTexture;

	//Objects
	PhysicsObject* player;
	PhysicsObject* testObj;
	PhysicsObject* wall;
	std::vector<PhysicsObject*> crateObjects;
	std::vector<PhysicsObject*> wallObjects;

	//SFML
	sf::RenderWindow *window;

	// Flags

	bool isDebugDrawOn;

};

