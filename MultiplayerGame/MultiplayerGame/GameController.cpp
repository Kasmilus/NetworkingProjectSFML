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
	//contactListener = new ContactListener;
	//physicsWorld->SetContactListener(contactListener);

	// Create objects
	SpawnObjects();

	// Load textures
	testTexture.loadFromFile("../resources/test.png");
	playerTexture.loadFromFile("../resources/player.png");
	wallTexture.loadFromFile("../resources/wall.png");
	wallTexture.setRepeated(true);
	crateTexture.loadFromFile("../resources/crate.png");

	// Assign textures
	player->SetTexture(playerTexture);
	testObj->SetTexture(testTexture);
	wall->SetTexture(wallTexture);
	for (auto &crate : crateObjects)
	{
		crate->SetTexture(crateTexture);
	}
	for (auto &wall : wallObjects)
	{
		wall->SetTexture(crateTexture);
	}

	isDebugDrawOn = false;
}

void GameController::CleanUp()
{
	// Objects
	if (wall)
		delete wall;
	if (player)
		delete player;
	if (testObj)
		delete testObj;

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
}

void GameController::SpawnObjects()
{
	player = new Player(physicsWorld, true, -15, 50, 3, 3);
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
		PhysicsObject* newCrate = new PhysicsObject(physicsWorld, true, posX, posY, size, size);
		crateObjects.push_back(newCrate);

		numberOfCrates--;
	}
}

bool GameController::Update(float deltaTime)
{

	Timer::Instance().Update(deltaTime);
	Input::Instance().Update();

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

	player->Update();
	testObj->Update();
	wall->Update();
	for (auto &crate : crateObjects)
	{
		crate->Update();
	}
	for (auto &wall : wallObjects)
	{
		wall->Update();
	}

	return true;
}

void GameController::Render()
{
	//physicsWorld->DrawDebugData();
	window->draw(*player->GetSprite());
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
	// Debug draw
	if(isDebugDrawOn)
		physicsWorld->DrawDebugData();

}
