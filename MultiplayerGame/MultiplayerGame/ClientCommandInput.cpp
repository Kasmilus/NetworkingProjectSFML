#include "ClientCommandInput.h"



ClientCommandInput::ClientCommandInput()
{
	horizontalInput = 0;
	verticalInput = 0;
	tauntButtonDown = false;
	spacePressed = false;
	spacePressedLastFrame = false;
	UpdateInput(0, 0, false);
}


ClientCommandInput::~ClientCommandInput()
{
}

void ClientCommandInput::Update()
{
	float deltaTime = Timer::Instance().GetDeltaTime();

	// Horizontal
	if (leftPressed)
	{
		horizontalInput -= SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (horizontalInput > 0)
			horizontalInput = 0;
	}
	else if (rightPressed)
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
	if (downPressed)
	{
		verticalInput += SENSITIVITY * deltaTime;
		// Snap to 0 after changing dir
		if (verticalInput < 0)
			verticalInput = 0;
	}
	else if (upPressed)
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
}

void ClientCommandInput::UpdateInput(float horInput, float verInput, bool actionButtonDown)
{
	if (horInput == 0)
	{
		leftPressed = false;
		rightPressed = false;
	}
	else
	{
		leftPressed = horInput < 0 ? true : false;
		rightPressed = !leftPressed;
	}

	if (verInput == 0)
	{
		upPressed = false;
		downPressed = false;
	}
	else
	{
		upPressed = verInput > 0 ? true : false;
		downPressed = !upPressed;
	}

	this->spacePressedLastFrame = this->spacePressed;
	this->spacePressed = actionButtonDown;
}
