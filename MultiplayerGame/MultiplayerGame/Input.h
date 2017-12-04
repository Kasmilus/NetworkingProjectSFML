#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "Timer.h"

enum KeyName { left, right, up, down, action, taunt};

class Input
{
public:
	static Input& Instance();

	virtual void Update();
	inline void BlockInput() { blockInput = true; }	// blocks input when window is not in focus

	bool isKeyPressed(KeyName key);
	float HorizontalInput() {return horizontalInput; }
	float VerticalInput() { return verticalInput; }
	bool IsSpaceDown() { return (spacePressed && !spacePressedLastFrame); }
	bool IsSpacePressed() { return spacePressed; }
	bool IsSpaceReleased() { return (!spacePressed && spacePressedLastFrame); }
	bool IsQuitPressed();
	bool IsDebugDrawDown() { return debugDrawDown && !debugDrawDownLastFrame; }
	bool IsTauntButtonPressed() { return tauntButtonDown; }

protected:
	Input() {}
	Input(Input const&);             
	void operator=(Input const&);

	// Control hor/vert input
	const float SENSITIVITY = 0.4f;
	const float GRAVITY = 10.f;

	// Keys
	bool spacePressed;
	bool spacePressedLastFrame;
	bool debugDrawDown;
	bool debugDrawDownLastFrame;
	bool tauntButtonDown;

	float horizontalInput;
	float verticalInput;

private:
	// Called only on client, server shouldn't care
	void UpdateClient();

	bool blockInput = false;

};

