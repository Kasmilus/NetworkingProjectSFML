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
#include "ConnectionInfo_Client.h"
#include "ConnectionInfo_Server.h"

enum GameState { WaitingToChooseNetworkingType, WaitingToStart, InGame, Finished};
enum NetworkingType { Server, Client};

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
	void UpdateGame(float deltaTime);
	void UpdateNetworking();

	void SpawnObjects();	// Randomly generates level with players
	void AssignTextures();
	bool CheckWinningConditions();	// Called after each player's death, returns true if round's finished

	void StartAsClient();
	void StartAsServer();
	void HandleClientConnection();
	void HandleServerConnection();

public:
	bool isWindowInFocus;

private:
	// Networking
	const float NETWORK_TIMESTEP = 1.0f / 20.0f;	// Rate at which information is exchanged between cleints and server. Simulation update rate is set in main.cpp
	float networkUpdateTimer;
	NetworkingType myNetworkingType;
	ConnectionInfo_Client connectionInfoClient;	// Used if this instance is client
	ConnectionInfo_Server connectionInfoServer;	// used if this instance is server
	std::vector<ClientState*> clientStates;	// Connected players game info

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
	GameState gameState;

	// Multiplayer
	std::vector<Player*> playerControllersList;
	const int NUMBER_OF_CLIENTS_MAX = 4;

};

