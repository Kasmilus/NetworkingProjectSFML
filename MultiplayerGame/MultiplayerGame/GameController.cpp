#include "GameController.h"



GameController::GameController()
{
}


GameController::~GameController()
{
}

void GameController::Init()
{
	// Physics
	b2Vec2 gravity(0.0f, 1.0f);	physicsWorld = new b2World(gravity);
	//contactListener = new ContactListener;
	//physicsWorld->SetContactListener(contactListener);

	// Create objects
	testObj = new PhysicsObject(physicsWorld, true, 0, 0, 1, 1);
	wall = new PhysicsObject(physicsWorld, false, 18, 28, 20, 1);

	// Load textures
	sf::Texture testTexture;
	testObj->SetTexture("../resources/test.png");	// BAD SOLUTION, WHAT IF I WANT TO REUSE TEXTURE?
	wall->SetTexture("../resources/test.png");

}

void GameController::CleanUp()
{
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

void GameController::Render(sf::RenderWindow &window)
{
	window.draw(*testObj->GetSprite());
	window.draw(*wall->GetSprite());
}
