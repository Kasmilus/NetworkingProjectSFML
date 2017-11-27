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
#include "CrateObject.h"

// Connected player struct
// Player IP PORT
// Points

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
	void SpawnObjects();	// Randomly generates level with players
	void AssignTextures();
	bool CheckWinningConditions();	// Called after each player's death, returns true if round's finished

	void StartAsClient();
	void StartAsServer();

private:
	// Physics
	b2World* physicsWorld;
	ContactListener* contactListener;
	SFMLDebugDraw* debugDraw;	// debug drawing
	const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const int VEL_ITERATIONS = 8;
	const int POS_ITERATIONS = 3;

	// Text
	sf::Font font;
	sf::Text bigText;
	sf::Text smallText;
	sf::Text playerNameText[4];
	sf::Text playerScoreText[4];

	//Textures
	sf::Texture testTexture;
	sf::Texture playerTexture;
	sf::Texture wallTexture;
	sf::Texture crateTexture;

	//Objects
	PhysicsObject* testObj;
	PhysicsObject* wall;
	std::vector<CrateObject*> crateObjects;
	std::vector<PhysicsObject*> wallObjects;

	//SFML
	sf::RenderWindow *window;

	// Flags
	bool isDebugDrawOn;
	bool isGameFinished;

	// Multiplayer
	// vector<ConnectedPlayerStruct> playersList;
	std::vector<Player*> playerControllersList;

};

