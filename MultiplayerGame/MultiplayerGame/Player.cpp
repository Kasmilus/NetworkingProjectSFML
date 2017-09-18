#include "Player.h"



Player::Player(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
	:PhysicsObject(physicsWorld, isDynamic, posX, posY, sizeX, sizeY)
{
}


Player::~Player()
{
}

void Player::Update()
{
	move();

	PhysicsObject::Update();
}

void Player::move()
{
	float speed = 0.7f;
	float horInput = Input::Instance().HorizontalInput();
	if (horInput != 0)
		horInput = horInput > 0 ? 1 : -1;
	float verInput = Input::Instance().VerticalInput();
	if (verInput != 0)
		verInput = verInput > 0 ? 1 : -1;
	physicsBody->SetLinearVelocity(b2Vec2(horInput*speed, -verInput*speed));
}
