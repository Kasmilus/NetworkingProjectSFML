#pragma once
#include "PhysicsObject.h"

class Player :
	public PhysicsObject
{
public:
	Player(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~Player();

	void Update() override;

private:
	void move();
};

