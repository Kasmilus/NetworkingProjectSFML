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
	b2Vec2 gravity(0.0f, 0.0f);	b2World world(gravity);

	// Load textures
	if (!testTexture.loadFromFile("../resources/test.png"))
	{
		// error...
	}
	testSprite.setTexture(testTexture);

}

void GameController::CleanUp()
{
}

bool GameController::Update(float deltaTime)
{	
	Timer::Instance().Update(deltaTime);
	Input::Instance().Update();

	if (Input::Instance().IsQuitPressed())
	{
		return false;
	}

	testSprite.setPosition(testSprite.getPosition() + sf::Vector2f(Input::Instance().HorizontalInput(), Input::Instance().VerticalInput()));

	return true;
}

void GameController::Render(sf::RenderWindow &window)
{
	window.draw(testSprite);
}
