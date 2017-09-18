#include "GameController.h"



GameController::GameController()
{
}


GameController::~GameController()
{
}

void GameController::Init(sf::RenderWindow *window)
{
	//SFML
	this->window = window;
	// Physics
	b2Vec2 gravity(0.0f, 0.0f);
	physicsWorld = new b2World(gravity);
	debugDraw = new SFMLDebugDraw(*window);
	physicsWorld->SetDebugDraw(debugDraw);
	debugDraw->SetFlags(b2Draw::e_shapeBit);
	//contactListener = new ContactListener;
	//physicsWorld->SetContactListener(contactListener);

	// Create objects
	testObj = new Player(physicsWorld, true, -15, 50, 3, 3);
	wall = new PhysicsObject(physicsWorld, false, 0, 10, 10, 10);

	// Load textures
	testTexture.loadFromFile("../resources/test.png");

	// Assign textures
	testObj->SetTexture(testTexture);
	wall->SetTexture(testTexture);

}

void GameController::CleanUp()
{
	// Objects
	if (wall)
		delete wall;
	if (testObj)
		delete testObj;
}

bool GameController::Update(float deltaTime)
{

	Timer::Instance().Update(deltaTime);
	Input::Instance().Update();

	// Physics world
	physicsWorld->Step(PHYSICS_TIMESTEP, VEL_ITERATIONS, POS_ITERATIONS);

	// Quitting game
	if (Input::Instance().IsQuitPressed())
	{
		return false;
	}

	testObj->Update();
	wall->Update();
	//testSprite.setPosition(testSprite.getPosition() + sf::Vector2f(Input::Instance().HorizontalInput(), Input::Instance().VerticalInput()));

	return true;
}

void GameController::Render()
{
	//physicsWorld->DrawDebugData();
	window->draw(*testObj->GetSprite());
	window->draw(*wall->GetSprite());
	physicsWorld->DrawDebugData();

}
