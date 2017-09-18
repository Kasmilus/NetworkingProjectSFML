#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "Timer.h"

class Input
{
public:
	static Input& Instance();

	void Update();

	float HorizontalInput() {return horizontalInput; }
	float VerticalInput() { return verticalInput; }
	bool IsSpaceDown() { return (spacePressed && !spacePressedLastFrame); }
	bool IsSpacePressed() { return spacePressed; }
	bool IsSpaceReleased() { return (!spacePressed && spacePressedLastFrame); }
	bool IsQuitPressed();

private:
	Input() {}
	Input(Input const&);              // Don't Implement.
	void operator=(Input const&); // Don't implement


	// Control hor/vert input
	const float SENSITIVITY = 0.4f;
	const float GRAVITY = 10.f;

	// Keys
	bool spacePressed;
	bool spacePressedLastFrame;
	float horizontalInput;
	float verticalInput;
};

