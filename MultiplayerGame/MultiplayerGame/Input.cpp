#include "Input.h"



Input & Input::Instance()
{
	static Input instance;
	return instance;
}

void Input::Update()
{
	float deltaTime = Timer::Instance().GetDeltaTime();

	// Horizontal
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		horizontalInput -= SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (horizontalInput > 0)
			horizontalInput = 0;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		horizontalInput += SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (horizontalInput < 0)
			horizontalInput = 0;
	}
	else
	{
		if (horizontalInput > 0.1f)
		{
			horizontalInput -= GRAVITY * deltaTime;
		}
		else if (horizontalInput < -0.1f)
		{
			horizontalInput += GRAVITY * deltaTime;
		}
		else
		{
			horizontalInput = 0;
		}
	}
	if (horizontalInput > 1) horizontalInput = 1;
	else if (horizontalInput < -1) horizontalInput = -1;

	// Vertical
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		verticalInput += SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (verticalInput < 0)
			verticalInput = 0;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		verticalInput -= SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (verticalInput > 0)
			verticalInput = 0;
	}
	else
	{
		if (verticalInput > 0.1f)
		{
			verticalInput -= GRAVITY * deltaTime;
		}
		else if (verticalInput < -0.1f)
		{
			verticalInput += GRAVITY * deltaTime;
		}
		else
		{
			verticalInput = 0;
		}
	}
	if (verticalInput > 1) verticalInput = 1;
	else if (verticalInput < -1) verticalInput = -1;

	// Space
	spacePressedLastFrame = spacePressed;
	spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

	// Debug draw
	debugDrawDownLastFrame = debugDrawDown;
	debugDrawDown = sf::Keyboard::isKeyPressed(sf::Keyboard::I);

}

bool Input::IsQuitPressed()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		return true;
	}

	return false;
}