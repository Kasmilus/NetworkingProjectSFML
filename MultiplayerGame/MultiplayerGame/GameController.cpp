#include "GameController.h"



GameController::GameController()
{
}


GameController::~GameController()
{
}

void GameController::Init(sf::RenderWindow *window)
{
	// Random seed
	srand(time(NULL));

	//SFML
	this->window = window;

	// Physics
	b2Vec2 gravity(0.0f, 0.0f);
	physicsWorld = new b2World(gravity);
	contactListener = new ContactListener();
	physicsWorld->SetContactListener(contactListener);
	debugDraw = new SFMLDebugDraw(*window);
	physicsWorld->SetDebugDraw(debugDraw);
	debugDraw->SetFlags(b2Draw::e_shapeBit);

	// Networking
	connectionInfoServer.PassClientStatesPointer(&clientStates);

	// Text
	font.loadFromFile("../resources/BebasNeue.otf");
	for (int i = 0; i < 4; ++i)
	{
		// Player name
		playerNameText[i].setFont(font);
		playerNameText[i].setString("P" + std::to_string(i + 1));
		playerNameText[i].setCharacterSize(30);
		playerNameText[i].setScale(0.2f, -0.2f);
		playerNameText[i].setColor(sf::Color::Yellow);
		playerNameText[i].setOrigin(playerNameText[i].getLocalBounds().left, playerNameText[i].getLocalBounds().top);
		float viewWidth = window->getView().getSize().x;
		playerNameText[i].setPosition(i * viewWidth / 4 - viewWidth / 2 + i * 5, 4.5f);

		// Player score
		playerScoreText[i].setFont(font);
		playerScoreText[i].setString(std::to_string(0));
		playerScoreText[i].setCharacterSize(30);
		playerScoreText[i].setScale(0.18f, -0.18f);
		playerScoreText[i].setColor(sf::Color::White);
		playerScoreText[i].setOrigin(playerScoreText[i].getLocalBounds().left, playerScoreText[i].getLocalBounds().top);
		playerScoreText[i].setPosition(i * viewWidth / 4 - viewWidth / 2 + 8 + i * 5, 4);
	}
	// Big text
	bigText.setFont(font);
	bigText.setString("Waiting to start");
	bigText.setCharacterSize(50);
	bigText.setScale(0.2f, -0.2f);
	bigText.setColor(sf::Color::Yellow);
	bigText.setPosition(-window->getView().getSize().x / 3 + 8, 50);
	// Small text
	smallText.setFont(font);
	smallText.setString("Press A to start a server, D to join as a client.");
	smallText.setCharacterSize(30);
	smallText.setScale(0.15f, -0.15f);
	smallText.setColor(sf::Color::White);
	smallText.setPosition(-window->getView().getSize().x / 3, 40);

	// Load textures
	testTexture.loadFromFile("../resources/test.png");
	playerTexture.loadFromFile("../resources/player.png");
	wallTexture.loadFromFile("../resources/wall.png");
	wallTexture.setRepeated(true);
	crateTexture.loadFromFile("../resources/crate.png");

	testObj = nullptr;
	wall = nullptr;

	isDebugDrawOn = false;
	gameState = GameState::WaitingToChooseNetworkingType;
	networkUpdateTimer = 0.0f;
}

void GameController::CleanUp()
{
	// Objects
	if (wall)
	{
		delete wall;
		wall = 0;
	}

	for (auto &player : playerControllersList)
	{
		if (player)
			delete player;
	}
	playerControllersList.clear();

	if (testObj)
	{
		delete testObj;
		testObj = 0;
	}

	for (auto &crate : crateObjects)
	{
		delete crate;
	}
	crateObjects.clear();

	for (auto &wall : wallObjects)
	{
		delete wall;
	}
	wallObjects.clear();

	delete physicsWorld;
}

void GameController::GenerateLevel()
{
	numberOfSpawnedObjects = 0;
	// Spawn players
	int playerStartPos[4][2]{ {-30, 15}, {-30, 75}, {30, 15}, {30, 75} };
	for (int i = 0; i < connectionInfoServer.GetConnectedClientsNumber(); ++i)	// later change 4 to number of connected players
	{
		Player* newPlayerController = new Player(physicsWorld, &font, true, playerStartPos[i][0], playerStartPos[i][1], 3, 3);
		newPlayerController->SetID(numberOfSpawnedObjects);
		if (i == 0)
			newPlayerController->canMove = true;
		playerControllersList.push_back(newPlayerController);
		numberOfSpawnedObjects++;
	}

	SpawnStaticLevel();

	// Generate number between 15-30
	int numberOfCrates = rand() % 15 + 1 + 15;
	// Generate crates
	while (numberOfCrates > 0)
	{
		// Random size between 2-4
		float size = 2.5f + (rand() % 10 + 1) / 5 * 2;
		float posX = -50 + (rand() % 81 + 1);
		float posY = (rand() % 80 + 1);
		CrateObject* newCrate = new CrateObject(physicsWorld, true, posX, posY, size, size);
		newCrate->SetID(numberOfSpawnedObjects);
		crateObjects.push_back(newCrate);

		numberOfSpawnedObjects++;
		numberOfCrates--;
	}

	AssignTextures();
}

void GameController::SpawnStaticLevel()
{
	testObj = new PhysicsObject(physicsWorld, true, -10, 80, 3, 3);
	testObj->SetID(numberOfSpawnedObjects);
	numberOfSpawnedObjects++;

	// Walls
	wall = new PhysicsObject(physicsWorld, false, 0, 30, 5, 60);
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, -50, 50, 5, 100));
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, 50, 50, 5, 100));
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, 0, 0, 100, 5));
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, 0, 100, 100, 5));
}

std::vector<ServerUpdatePacket> GameController::SpawnLevelForClient(sf::Packet packet, uint8 numberOfObjects)
{
	SpawnStaticLevel();

	// Unpack the data
	std::vector<ServerUpdatePacket> updates;
	for (int i = 0; i < numberOfObjects; ++i)
	{
		ServerUpdatePacket update;
		packet >> update;
		updates.push_back(update);
	}

	for (ServerUpdatePacket updatePacket : updates)
	{
		ServerUpdateType updateType = (ServerUpdateType)updatePacket.updateType;
		int id = updatePacket.objectID;
		bool isPlayer = updatePacket.fieldThree;
		float posX = updatePacket.fieldOne;
		float posY = updatePacket.fieldTwo;
		float size = updatePacket.fieldFour;
		if (updateType == ServerUpdateType::PlayerCharacter)
		{
			Player* newPlayerController = new Player(physicsWorld, &font, true, posX, posY, 3, 3);
			newPlayerController->SetID(id);
			newPlayerController->canMove = true;
			playerControllersList.push_back(newPlayerController);
		}
		else
		{
			if (isPlayer)
			{
				Player* newPlayerController = new Player(physicsWorld, &font, true, posX, posY, 3, 3);
				newPlayerController->SetID(id);
				playerControllersList.push_back(newPlayerController);
			}
			else
			{
				CrateObject* newCrate = new CrateObject(physicsWorld, true, posX, posY, size, size);
				newCrate->SetID(id);
				crateObjects.push_back(newCrate);
			}
		}
	}

	AssignTextures();

	return updates;
}

void GameController::AssignTextures()
{
	testObj->SetTexture(testTexture);
	wall->SetTexture(wallTexture);
	for (auto &player : playerControllersList)
	{
		player->SetTexture(playerTexture);
	}
	for (auto &crate : crateObjects)
	{
		crate->SetTexture(crateTexture);
	}
	for (auto &wall : wallObjects)
	{
		wall->SetTexture(crateTexture);
	}
}

bool GameController::CheckWinningConditions()
{
	if (playerControllersList.size() == 1)
	{
		// Add point to that player
		// ...
		// Delete last player controller
		delete playerControllersList.back();
		playerControllersList.clear();
		// Delete whole level
		for (auto &crate : crateObjects)
		{
			delete crate;
		}
		crateObjects.clear();
		for (auto &wall : wallObjects)
		{
			delete wall;
		}
		wallObjects.clear();

		// Check if any player has 3 points
		// if so print the winner(for all players) and ask if start new match(just on server)
		// if not start new round:

		GenerateLevel();
		AssignTextures();

		return true;
	}

	return false;
}

PhysicsObject * GameController::FindObjectByID(sf::Uint8 ID)
{
	for (auto& player : playerControllersList)
	{
		if (player->GetID() == ID)
			return player;
	}
	for (auto& crate : crateObjects)
	{
		if (crate->GetID() == ID)
			return crate;
	}

	LOG(ERROR, INGAME) << "There's no object with that ID! Something may explode.";
	return nullptr;
}

void GameController::StartAsClient()
{
	// Set flags
	myNetworkingType = NetworkingType::Client;
	gameState = GameState::WaitingToStart;

	// Bind socket
	connectionInfoClient.BindUDPSocket();

	// Send first message to the server
	sf::Packet handshakePacket = connectionInfoClient.CreateHandshakePacket();
	connectionInfoClient.ConnectToServer();

	LOG(INFO) << "Trying to connect with server.";
}

void GameController::StartAsServer()
{
	// Set flags
	myNetworkingType = NetworkingType::Server;
	gameState = GameState::WaitingToStart;

	// Bind socket
	connectionInfoServer.BindUDPSocket();
	LOG(INFO) << "Server starts listening for connections.";
}

void GameController::HandleClientConnection()
{
	sf::Packet receivedPacket;

	ConnectionStatus status = connectionInfoClient.GetConnectionStatus();
	switch (status) {
	case ConnectionStatus::Disconnected:
		// Client disconnected for some reason - go back to main screen
		gameState = GameState::WaitingToChooseNetworkingType;
		break;
	case ConnectionStatus::NoConnection:
		// Try to connect with server
		if (connectionInfoClient.ConnectToServer())
		{
			// Connected - say hello
			receivedPacket = connectionInfoClient.CreateHandshakePacket();
			connectionInfoClient.SendPacketTCP(receivedPacket);
			LOG(INFO) << "Sending first message to the server.";
		}
		break;
	case ConnectionStatus::ConnectionUnconfirmed:
		// Connection was established but client is still waiting for confirmation

		// Check if server accepted connection request
		if (connectionInfoClient.ReceivePacketTCP(receivedPacket))
		{
			// Received confirmation packet
			HandshakePacket handshakePacketInfo;
			receivedPacket >> handshakePacketInfo;

			// Check if server accepted this client
			if (handshakePacketInfo.accepted)
			{
				connectionInfoClient.ConnectionWithServerAccepted();
				LOG(INFO) << "Server accepted connection, waiting for game to start.";
			}
			else
			{
				// This should print some pretty message
				// but to make things simple we're going to set state back to choosing between server/client
				LOG(ERROR) << "Server didn't accept connection!";

				connectionInfoClient.Disconnect();
				gameState = GameState::WaitingToChooseNetworkingType;
			}
		}
		break;
	case ConnectionStatus::ConnectionConfirmed:
		// Wait for "game start" message
		if (connectionInfoClient.ReceivePacketTCP(receivedPacket))
		{
			// Unpack data
			ServerGameStateCommand cmd = UnpackServerPacket(receivedPacket);
			if (cmd == ServerGameStateCommand::StartGame)
			{
				LOG(INFO, NETWORK) << "Received game start message!";
				gameState = GameState::InGame;
			}
		}
		break;
	}
}

void GameController::HandleServerConnection()
{
	// Listen for connections, accepted clients will be added to clientStates
	connectionInfoServer.ListenForConnections();
	// On space press start the game
	bool startTheGame = Input::Instance().IsSpacePressed();

	if (startTheGame)
	{
		GenerateLevel();
	}

	for (int id = 0; id < clientStates.size(); ++id)
	{
		sf::Packet receivedPacket;
		sf::Packet responsePacket;

		ConnectionStatus connectionStatus = clientStates.at(id)->GetConnectionStatus();
		switch (connectionStatus) {
		case ConnectionStatus::Disconnected:
			LOG(WARNING) << "Client disconnected.";
			clientStates.erase(clientStates.begin() + id);
			--id;
			break;
		case ConnectionStatus::NoConnection:
			// This client was connected but isn't anymore
			// Purpose of this class is to keep that information in case of reconnect but for now just throw it away, implement properly later!
			LOG(WARNING) << "Removing disconnected player from the list!";
			clientStates.erase(clientStates.begin() + id);
			--id;
			break;
		case ConnectionStatus::ConnectionUnconfirmed:
			if (connectionInfoServer.ReceivePacketTCP(responsePacket, id))
			{
				// Received confirmation packet
				HandshakePacket handshakePacketInfo;
				responsePacket >> handshakePacketInfo;

				LOG(INFO) << "Received first packet from client";
				// Check if server can accept new player
				if (clientStates.size() > NUMBER_OF_CLIENTS_MAX)
				{
					// Decline connection
					responsePacket = connectionInfoServer.CreateHandshakePacket(false);
					connectionInfoServer.CloseConnectionWithClient(id);
					LOG(INFO) << "Max number of players connected, declining new connection attempt.";
				}
				else
				{
					// Accept connection
					responsePacket = connectionInfoServer.CreateHandshakePacket(true);
					connectionInfoServer.SendPacketTCP(responsePacket, id);
					clientStates.at(id)->SetConnected();
					LOG(INFO) << "Player with ID " << id << " joined the server.";
				}
			}

			break;
		case ConnectionStatus::ConnectionConfirmed:
			if (startTheGame)
			{
				// Tell clients that game starts now
				PackServerPacket(responsePacket, id, ServerGameStateCommand::StartGame);
				connectionInfoServer.SendPacketTCP(responsePacket, id);
				LOG(INFO) << "Game start message sent to player with ID " << id << ".";
			}
			break;
		}
	}

	if (startTheGame)
	{
		gameState = GameState::InGame;
		clientCommandsHistory.resize(connectionInfoServer.GetConnectedClientsNumber());
	}
}

void GameController::PackClientPacket(sf::Packet & packet)
{
	ClientPacket clientPacket;
	clientPacket.timestamp = Timer::Instance().GetSimulationTime();

	// Check for any ClientSimulationUpdate changes since last NetworkUpdate()
	//..
	clientPacket.objectID = 0;	//
	clientPacket.clientSimulationUpdate = ClientSimulationUpdate::Nothingg;	// isn't that waste of bandwith? Usually nothing will be sent.

	// Check for any input commands to send
	std::vector<ClientActionCommand> commands;
	Input& input = Input::Instance();
	if (input.HorizontalInput() > 0)
	{
		commands.push_back(ClientActionCommand::MoveRight);
	}
	else if (input.HorizontalInput() < 0)
	{
		commands.push_back(ClientActionCommand::MoveLeft);
	}
	if (input.VerticalInput() > 0)
	{
		commands.push_back(ClientActionCommand::MoveUp);
	}
	else if (input.VerticalInput() < 0)
	{
		commands.push_back(ClientActionCommand::MoveDown);
	}
	if (input.IsSpacePressed())
	{
		commands.push_back(ClientActionCommand::PressActionButton);
	}
	clientPacket.numberOfCommands = commands.size();

	// Pack all the information into the packet
	packet << clientPacket;
	for (int i = 0; i < commands.size(); ++i)
	{
		packet << commands.at(i);
	}
}

void GameController::UnpackClientPacket(sf::Packet & packet, sf::Uint8 clientID)
{
	ClientPacket clientPacket;
	packet >> clientPacket;

	if (clientCommandsHistory.at(clientID).size() > 0)
	{
		// First check if this packet arrived in order/isn't a duplicate
		if (clientCommandsHistory.at(clientID).front()->first >= clientPacket.timestamp)
		{
			return;
		}
	}

	// Unpack the packet
	std::vector<ClientActionCommand> commands;
	for (int i = 0; i < clientPacket.numberOfCommands; ++i)
	{
		ClientActionCommand command;
		sf::Uint8 readCommand;
		packet >> readCommand;
		command = (ClientActionCommand)readCommand;
		commands.push_back(command);
	}

	std::pair<float, std::vector<ClientActionCommand>>* newCommands = new std::pair<float, std::vector<ClientActionCommand>>();
	newCommands->first = clientPacket.timestamp;
	newCommands->second = commands;
	clientCommandsHistory.at(clientID).push_front(newCommands);
}

void GameController::PackServerPacket(sf::Packet & packet, sf::Uint8 clientID, ServerGameStateCommand message)
{
	ServerPacket serverPacket;
	serverPacket.timestamp = Timer::Instance().GetSimulationTime();
	serverPacket.serverGameStateCommand = (sf::Uint8)message;
	
	// Save data needed to be sent
	vector<ServerUpdatePacket> updates;
	if (message == ServerGameStateCommand::StartGame)
	{
		PackServerGameStartData(updates, clientID);
	}
	else if (message == ServerGameStateCommand::Nothing)
	{
		PackServerGameStateChanges(updates);
	}

	// Pack the data
	serverPacket.numberOfUpdates = updates.size();
	packet << serverPacket;
	for (int i = 0; i < updates.size(); ++i)
	{
		packet << updates.at(i);
	}
}

void GameController::PackServerGameStateChanges(vector<ServerUpdatePacket>& updates)
{
	// Check what information exactly player needs
	// ..(for now just send everything)
	for (auto& crate : crateObjects)
	{
		ServerUpdatePacket newUpdate;
		newUpdate.objectID = crate->GetID();
		if (crate->IsMarkedForDestruction())
		{
			newUpdate.updateType = ServerUpdateType::Destroy;
			newUpdate.fieldThree = true;
		}
		else
		{
			newUpdate.updateType = ServerUpdateType::Position;
			newUpdate.fieldOne = crate->GetPhysicsBody()->GetPosition().x;
			newUpdate.fieldTwo = crate->GetPhysicsBody()->GetPosition().y;
		}

		updates.push_back(newUpdate);
	}
	for (auto& player : playerControllersList)
	{
		ServerUpdatePacket newUpdate;
		newUpdate.objectID = player->GetID();
		newUpdate.updateType = ServerUpdateType::Position;
		newUpdate.fieldOne = player->GetPhysicsBody()->GetPosition().x;
		newUpdate.fieldTwo = player->GetPhysicsBody()->GetPosition().y;

		if (player->IsMarkedForDestruction())
		{
			newUpdate.updateType = ServerUpdateType::Destroy;
			newUpdate.fieldThree = true;
		}

		updates.push_back(newUpdate);
	}
}

void GameController::PackServerGameStartData(vector<ServerUpdatePacket>& updates, sf::Uint8 clientID)
{
	for (auto& crate : crateObjects)
	{
		ServerUpdatePacket newUpdate;
		newUpdate.objectID = crate->GetID();

		newUpdate.updateType = ServerUpdateType::Position;
		newUpdate.fieldOne = crate->GetPhysicsBody()->GetPosition().x;
		newUpdate.fieldTwo = crate->GetPhysicsBody()->GetPosition().y;
		newUpdate.fieldThree = false;
		newUpdate.fieldFour = crate->GetSize();

		updates.push_back(newUpdate);
	}

	for (int i = 0; i < playerControllersList.size(); ++i)
	{
		Player* player = playerControllersList.at(i);
		ServerUpdatePacket newUpdate;
		newUpdate.objectID = player->GetID();
		if (clientID == i)	// Tell player if it's his character
			newUpdate.updateType = ServerUpdateType::PlayerCharacter;
		else
			newUpdate.updateType = ServerUpdateType::Position;

		newUpdate.fieldOne = player->GetPhysicsBody()->GetPosition().x;
		newUpdate.fieldTwo = player->GetPhysicsBody()->GetPosition().y;
		newUpdate.fieldThree = true;
		newUpdate.fieldFour = 0;	// Doesn't matter for a player

		updates.push_back(newUpdate);
	}
}

ServerGameStateCommand GameController::UnpackServerPacket(sf::Packet & packet)
{
	ServerPacket serverPacket;
	std::vector<ServerUpdatePacket> updates;
	packet >> serverPacket;
	ServerGameStateCommand command = (ServerGameStateCommand)serverPacket.serverGameStateCommand;

	// Check if it's start game message, if it is - spawn all received objects
	if (command == ServerGameStateCommand::StartGame)
	{
		updates = SpawnLevelForClient(packet, serverPacket.numberOfUpdates);
	}
	else if (command != ServerGameStateCommand::Nothing)
	{
		// Rest of the message isn't relevant - we don't need to update objects
		return (ServerGameStateCommand)command;
	}
	else
	{
		// Check if this packet arrived in order/isn't a duplicate
		if (serverPacket.timestamp <= snapshots.back()->first)
		{
			LOG(WARNING, INGAME) << "timestamp: " << serverPacket.timestamp;
			return ServerGameStateCommand::Nothing;
		}

		// Unpack the packet
		for (int i = 0; i < serverPacket.numberOfUpdates; ++i)
		{
			ServerUpdatePacket update;
			packet >> update;
			updates.push_back(update);
		}
	}
	

	std::pair<float, std::vector<ServerUpdatePacket>>* newSnapshot = new std::pair<float, std::vector<ServerUpdatePacket>>();
	newSnapshot->first = serverPacket.timestamp;
	newSnapshot->second = updates;
	snapshots.push_front(newSnapshot);

	return (ServerGameStateCommand)serverPacket.serverGameStateCommand;
}

bool GameController::Update(float deltaTime)
{
	Timer::Instance().Update(deltaTime);
	networkUpdateTimer += deltaTime;

	if (isWindowInFocus)
	{
		Input::Instance().Update();
	}

	switch (gameState)
	{
	case GameState::WaitingToChooseNetworkingType:
		// Start as server/client
		if (Input::Instance().HorizontalInput() > 0)	// Client
			StartAsClient();
		else if (Input::Instance().HorizontalInput() < 0)	// Server
			StartAsServer();

		return true;
		break;
	case GameState::WaitingToStart:
		// Not connected yet(client)/not started the game yet(server)
		if (myNetworkingType == NetworkingType::Client)
		{
			// Establish connection with the server
			HandleClientConnection();
		}
		else
		{
			// Accept player connections
			HandleServerConnection();
		}
		return true;
		break;
	case GameState::InGame:
		// Physics world
		physicsWorld->Step(PHYSICS_TIMESTEP, VEL_ITERATIONS, POS_ITERATIONS);

		// Debug draw
		if (Input::Instance().IsDebugDrawDown())
		{
			isDebugDrawOn = !isDebugDrawOn;
		}

		if (myNetworkingType == NetworkingType::Client)
		{
			// Check if any packets arrived from the server
			sf::Packet serverPacket;
			if(connectionInfoClient.ReceivePacketUDP(serverPacket))
				UnpackServerPacket(serverPacket);

			// Interp/exterp other objects positions and predict player position
			SimulateGame(deltaTime);
			UpdateGame(deltaTime);
		}
		else
		{
			for (int id = 0; id < playerControllersList.size(); ++id)
			{
				sf::Packet clientPacket;
				if (connectionInfoServer.ReceivePacketUDP(clientPacket, id))
					UnpackClientPacket(clientPacket, id);
			}

			// Update game simulation
			UpdateGame(deltaTime);
		}

		// Send update to client/server
		if (networkUpdateTimer >= NETWORK_TIMESTEP)
		{
			networkUpdateTimer = 0.0f;
			UpdateNetworking();
		}
		break;
	case GameState::Finished:
		if (myNetworkingType == NetworkingType::Server)
		{
			if (Input::Instance().IsSpacePressed())
			{
				//Restart the game
			}
		}
		break;
	}


	// Quitting game
	if (Input::Instance().IsQuitPressed())
	{
		return false;
	}

	return true;
}

void GameController::UpdateGame(float deltaTime)
{
	// Update objects
	for (auto &player : playerControllersList)
	{
		player->Update();
	}
	testObj->Update();
	wall->Update();
	for (auto &wall : wallObjects)
	{
		wall->Update();
	}

	// Destroy marked objects
	if (crateObjects.size() > 0)
	{
		for (std::vector<CrateObject*>::iterator crate = crateObjects.begin(); crate != crateObjects.end(); )
		{
			(*crate)->Update();
			if ((*crate)->IsMarkedForDestruction())
			{
				delete *crate;
				crate = crateObjects.erase(crate);
			}
			else
			{
				++crate;
			}
		}
	}

	if (playerControllersList.size() > 1)
	{
		for (std::vector<Player*>::iterator player = playerControllersList.begin(); player != playerControllersList.end(); )
		{
			if ((*player)->IsMarkedForDestruction())
			{
				delete (*player);
				player = playerControllersList.erase(player);

				if (CheckWinningConditions())
				{
					break;
				}
			}
			else
			{
				++player;
			}

		}
	}


}

void GameController::SimulateGame(float deltaTime)
{
	std::vector<ServerUpdatePacket>* updates;

	while (snapshots.size() > 2)
	{
		snapshots.pop_back();
	}
	
	updates = &snapshots.back()->second;

	// .. ( now just move to the position, later interpolate)
	for (ServerUpdatePacket updatePacket : *updates)
	{
		PhysicsObject* objectToUpdate = FindObjectByID(updatePacket.objectID);
		if (objectToUpdate)
		{
			switch (updatePacket.updateType)
			{
			case ServerUpdateType::Position:
				objectToUpdate->GetPhysicsBody()->SetTransform(b2Vec2(updatePacket.fieldOne, updatePacket.fieldTwo), objectToUpdate->GetPhysicsBody()->GetAngle());
				break;
			case ServerUpdateType::Destroy:
				objectToUpdate->Destroy();
				break;

			}
		}
	}
}

void GameController::UpdateNetworking()
{
	if (myNetworkingType == NetworkingType::Client)
	{
		sf::Packet clientPacket;
		PackClientPacket(clientPacket);
		connectionInfoClient.SendPacketUDP(clientPacket);
	}
	else
	{
		for (int id = 0; id < playerControllersList.size(); ++id)
		{
			sf::Packet serverPacket;
			PackServerPacket(serverPacket, id, ServerGameStateCommand::Nothing);
			connectionInfoServer.SendPacketUDP(serverPacket, id);
		}
	}
}

void GameController::Render()
{
	for (auto &player : playerControllersList)
	{
		window->draw(*player->GetSprite());
	}
	if (testObj)
		window->draw(*testObj->GetSprite());
	if (wall)
		window->draw(*wall->GetSprite());

	for (auto &crate : crateObjects)
	{
		window->draw(*crate->GetSprite());
	}
	for (auto &wall : wallObjects)
	{
		window->draw(*wall->GetSprite());
	}

	// Text
	for (int i = 0; i < playerControllersList.size(); ++i)
	{
		window->draw(playerNameText[i]);
		playerScoreText->setString(std::to_string(0));	// later get score from player objects
		window->draw(playerScoreText[i]);

		window->draw(playerControllersList[i]->tauntText);
	}

	if (gameState != GameState::InGame)
	{
		// Changing string every frame :( change that later
		if (gameState == GameState::WaitingToChooseNetworkingType)
		{
			smallText.setString("Press A to start a server, D to join as a client.");
		}
		if (gameState == GameState::WaitingToStart)
		{
			if (myNetworkingType == NetworkingType::Client)
				smallText.setString("Waiting for server to start the game.");
			else
				smallText.setString("Press space to start the game!");
		}

		window->draw(bigText);
		window->draw(smallText);
	}

	// Debug draw
	if (isDebugDrawOn)
		physicsWorld->DrawDebugData();

}
