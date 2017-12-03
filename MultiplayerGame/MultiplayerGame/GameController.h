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
	// Update - server
	void UpdateGame(float deltaTime);
	// Simulate - client
	void SimulateGame(float deltaTime);
	void UpdateNetworking();

	// Gameplay
	void GenerateLevel();	// Randomly generates objects and players
	void SpawnStaticLevel(bool onServer);	// Spawns walls, always at the same predefined positions
	std::vector<ServerUpdatePacket> SpawnLevelForClient(sf::Packet packet, uint8 numberOfObjects);	// Spawns level from data given by server and returns that(unpacked) data so it can be stored as first game state snapshot
	void AssignTextures();
	bool CheckWinningConditions();	// Called after each player's death, returns true if round's finished
	PhysicsObject* FindObjectByID(sf::Uint8 ID);

	// Networking
	void StartAsClient();
	void StartAsServer();
	void HandleClientConnection();
	void HandleServerConnection();
	void PackClientPacket(sf::Packet& packet);	// Client packing data to be sent to the server
	void UnpackClientPacket(sf::Packet& packet, sf::Uint8 clientID);	// Server unpacking data received from client
	void PackServerPacket(sf::Packet& packet, sf::Uint8 clientID, ServerGameStateCommand message = ServerGameStateCommand::Nothing);	// Server packing data to be sent to the client
	void PackServerGameStateChanges(vector<ServerUpdatePacket>& updates);
	void PackServerGameStartData(vector<ServerUpdatePacket>& updates, sf::Uint8 clientID);
	ServerGameStateCommand UnpackServerPacket(sf::Packet& packet);	// Client unpacking data received from server
	void PackChatPacketClient(sf::Packet& packet);
	void UnpackChatPacket(sf::Packet& packet);

public:
	bool isWindowInFocus;

private:
	// Networking - ideally a lot of that stuff should be in different class, now it's a bit messy
	const float NETWORK_TIMESTEP = 1.0f / 30.0f;	// Rate at which information is exchanged between cleints and server. Simulation update rate is set in main.cpp
	float networkUpdateTimer;
	NetworkingType myNetworkingType;
	ConnectionInfo_Client connectionInfoClient;	// Used if this instance is client
	ConnectionInfo_Server connectionInfoServer;	// used if this instance is server
	std::vector<ClientState*> clientStates;	// Connected players game info
	// For server:
	const float COMMAND_HISTORY_TIME = 0.6f;	// How long to keep history of player commands
	std::vector<std::list<std::pair<float, std::vector<ClientActionCommand>>*>> clientCommandsHistory;	// Time and commands - Vector of players keeping list of pair entries (time, commands) with history of commands
	// For client:
	std::list<std::pair<float, std::vector<ServerUpdatePacket>>*> snapshots;	// Time and all object updates
	const float CHAT_MESSAGE_INTERVAL = 1.5f;
	float chatMessageBlockTime;	// player can send new text message only if thats <= 0
	int myClientID;
	float serverSimulationStartTime;
	float clientServerClockDifference;
	float lastSentMessageTime;
	float roundTripTime;
	const float interp = 0.05f;

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
	uint8 numberOfSpawnedObjects;	// used to give IDs
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

