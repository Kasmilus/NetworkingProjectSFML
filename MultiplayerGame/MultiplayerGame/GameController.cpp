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

	// Text
	font.loadFromFile("../resources/BebasNeue.otf");
	for (int i = 0; i < 4; ++i)
	{
		// Player name
		playerNameText[i].setFont(font);
		playerNameText[i].setString("P" + std::to_string(i + 1));
		playerNameText[i].setCharacterSize(30);
		playerNameText[i].setScale(0.2f, 0.2f);
		playerNameText[i].setColor(sf::Color::Yellow);
		playerNameText[i].setOrigin(playerNameText[i].getLocalBounds().left, playerNameText[i].getLocalBounds().top);
		float viewWidth = window->getView().getSize().x;
		playerNameText[i].setPosition(i * viewWidth/4 - viewWidth/2 + i * 5, 0);

		// Player score
		playerScoreText[i].setFont(font);
		playerScoreText[i].setString(std::to_string(0));
		playerScoreText[i].setCharacterSize(30);
		playerScoreText[i].setScale(0.18f, 0.18f);
		playerScoreText[i].setColor(sf::Color::White);
		playerScoreText[i].setOrigin(playerScoreText[i].getLocalBounds().left, playerScoreText[i].getLocalBounds().top);
		playerScoreText[i].setPosition(i * viewWidth / 4 - viewWidth / 2 + 8 + i * 5, 0);
	}
	// Big text
	bigText.setFont(font);
	bigText.setString("Waiting to start");
	bigText.setCharacterSize(50);
	bigText.setScale(0.2f, 0.2f);
	bigText.setColor(sf::Color::Yellow);
	bigText.setPosition(-window->getView().getSize().x/3 + 8, 50);
	// Small text
	smallText.setFont(font);
	smallText.setString("Press A to start a server, D to join as a client.");
	smallText.setCharacterSize(30);
	smallText.setScale(0.15f, 0.15f);
	smallText.setColor(sf::Color::White);
	smallText.setPosition(-window->getView().getSize().x / 3, 40);

	// Load textures
	testTexture.loadFromFile("../resources/test.png");
	playerTexture.loadFromFile("../resources/player.png");
	wallTexture.loadFromFile("../resources/wall.png");
	wallTexture.setRepeated(true);
	crateTexture.loadFromFile("../resources/crate.png");

	// Level generation
	SpawnObjects();	
	AssignTextures();

	isDebugDrawOn = false;
	isGameFinished = true;
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

void GameController::SpawnObjects()
{
	int playerStartPos[4][2]{ {-30, 15}, {-30, 75}, {30, 15}, {30, 75} };
	for (int i = 0; i < 4; ++i)	// later change 4 to number of connected players
	{
		Player* newPlayerController = new Player(physicsWorld, &font, true, playerStartPos[i][0], playerStartPos[i][1], 3, 3);
		if (i == 0)
			newPlayerController->canMove = true;
		playerControllersList.push_back(newPlayerController);
	}
	testObj = new PhysicsObject(physicsWorld, true, -10, 80, 3, 3);

	// Walls
	wall = new PhysicsObject(physicsWorld, false, 0, 30, 5, 60);
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, -50, 50, 5, 100));
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, 50, 50, 5, 100));
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, 0, 0, 100, 5));
	wallObjects.push_back(new PhysicsObject(physicsWorld, false, 0, 100, 100, 5));


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
		crateObjects.push_back(newCrate);

		numberOfCrates--;
	}
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

		SpawnObjects();
		AssignTextures();

		return true;
	}
	
	return false;
}

void GameController::StartAsClient()
{

	isGameFinished = false;
}

void GameController::StartAsServer()
{

	isGameFinished = false;
}

bool GameController::Update(float deltaTime)
{

	Timer::Instance().Update(deltaTime);
	Input::Instance().Update();

	// Wait for server to start the game again
	if (isGameFinished)
	{
		if (Input::Instance().HorizontalInput() > 0)	// Client
			StartAsClient();
		else if (Input::Instance().HorizontalInput() < 0)
			StartAsServer();

		return true;
	}

	// Physics world
	physicsWorld->Step(PHYSICS_TIMESTEP, VEL_ITERATIONS, POS_ITERATIONS);

	// Debug draw
	if (Input::Instance().IsDebugDrawDown())
	{
		isDebugDrawOn = !isDebugDrawOn;
	}
	// Quitting game
	if (Input::Instance().IsQuitPressed())
	{
		return false;
	}

	for (auto &player : playerControllersList)
	{
		player->Update();
	}
	testObj->Update();
	wall->Update();

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

	for (auto &wall : wallObjects)
	{
		wall->Update();
	}

	return true;
}

void GameController::Render()
{
	for (auto &player : playerControllersList)
	{
		window->draw(*player->GetSprite());
	}
	window->draw(*testObj->GetSprite());
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

	if (isGameFinished)
	{
		window->draw(bigText);
		window->draw(smallText);
	}

	// Debug draw
	if(isDebugDrawOn)
		physicsWorld->DrawDebugData();

}
