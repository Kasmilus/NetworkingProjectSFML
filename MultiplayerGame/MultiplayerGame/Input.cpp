#include "Input.h"



Input & Input::Instance()
{
	static Input instance;
	return instance;
}

void Input::Update()
{
	float deltaTime = Timer::Instance().GetDeltaTime();
	blockInput = false;

	// Horizontal
	if (isKeyPressed(left))
	{
		horizontalInput -= SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (horizontalInput > 0)
			horizontalInput = 0;
	}
	else if(isKeyPressed(right))
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
	if (isKeyPressed(down))
	{
		verticalInput += SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (verticalInput < 0)
			verticalInput = 0;
	}
	else if (isKeyPressed(up))
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
	spacePressed = isKeyPressed(action);

	// Debug draw
	debugDrawDownLastFrame = debugDrawDown;
	debugDrawDown = sf::Keyboard::isKeyPressed(sf::Keyboard::I);

}

bool Input::isKeyPressed(KeyName key)
{
	if (blockInput)
		return false;

	if (key == KeyName::left)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			return true;
		else
			return false;
	}
	if (key == KeyName::right)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			return true;
		else
			return false;
	}
	if (key == KeyName::up)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			return true;
		else
			return false;
	}
	if (key == KeyName::down)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			return true;
		else
			return false;
	}
	if (key == KeyName::action)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			return true;
		else
			return false;
	}
	if (key == KeyName::taunt)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
			return true;
		else
			return false;
	}
}

bool Input::IsQuitPressed()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		return true;
	}

	return false;
}

void Input::UpdateClient()
{
}
