#include "Player.h"
#include "SFML\System\Vector2.hpp"
#include <math.h>  


Player::Player(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
	:PhysicsObject(physicsWorld, isDynamic, posX, posY, sizeX, sizeY)
{
	// Add sensor fixture to detect objects to pick up, players to punch etc.
	b2CircleShape shape;
	shape.m_radius = sizeX * 2;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.isSensor = true;
	physicsBody->CreateFixture(&fixtureDef);


	// Default values for variables
	isHoldingObject = false;
	throwPower = 1000;
	attackCharge = 0;
}


Player::~Player()
{
}

void Player::Update()
{
	move();

	if (Input::Instance().IsSpaceDown())
	{
		if (!isHoldingObject)
		{
			PhysicsObject* objectToPickUp = getObjectInRange();
			if (objectToPickUp != nullptr)
			{
				pickUpObject(objectToPickUp);
			}
		}
	}
	else if (Input::Instance().IsSpaceReleased())
	{
		if (isHoldingObject)
		{
			throwObject();
		}
		else
		{
			Player* enemyPlayer = getPlayerInRange();
			if (enemyPlayer != nullptr)
			{
				punchPlayer(enemyPlayer);
			}
		}
	}

	if (Input::Instance().IsSpacePressed())
	{
		// Charge attack
		attackCharge += Timer::Instance().GetDeltaTime();

		if (attackCharge > 3)
			attackCharge = 3;
	}

	PhysicsObject::Update();
}

void Player::BeginCollision(b2Fixture* coll, bool isTrigger)
{
	bool isDynamic = coll->GetBody()->GetType() == b2BodyType::b2_dynamicBody;

	if (isTrigger && isDynamic)
	{
		PhysicsObject* collObj = static_cast<PhysicsObject*>(coll->GetBody()->GetUserData());
		Player* enemyPlayer = dynamic_cast<Player*>(collObj);
		if (enemyPlayer)
		{
			playersInRange.push_back(enemyPlayer);
		}
		if (collObj)
		{
			objectsInRange.push_back(collObj);
		}
	}
	
}

void Player::EndCollision(b2Fixture* coll, bool isTrigger)
{
	bool isDynamic = coll->GetBody()->GetType() == b2BodyType::b2_dynamicBody;
	if (isTrigger && isDynamic)
	{
		PhysicsObject* collObj = static_cast<PhysicsObject*>(coll->GetBody()->GetUserData());
		Player* enemyPlayer = dynamic_cast<Player*>(collObj);
		if (enemyPlayer)
		{
			playersInRange.remove(enemyPlayer);
		}
		if (collObj)
		{
			objectsInRange.remove(collObj);
		}
	}
}

void Player::move()
{
	float speed = 0.4f;
	float horInput = Input::Instance().HorizontalInput();
	if (horInput != 0)
		horInput = horInput > 0 ? 1 : -1;
	float verInput = Input::Instance().VerticalInput();
	if (verInput != 0)
		verInput = verInput > 0 ? 1 : -1;

	// Don't allow moving vertically and horizontally and the same time
	if (horInput != 0)
		verInput = 0;

	physicsBody->SetLinearVelocity(b2Vec2(horInput*speed, -verInput*speed));

	physicsBody->SetAngularVelocity(0);

	float newAngle = lastFrameRotation;
	if (horInput > 0)
		newAngle = 0;
	else if (horInput < 0)
		newAngle = 180;
	else if (verInput > 0)
		newAngle = 270;
	else if (verInput < 0)
		newAngle = 90;

	if (lastFrameRotation != newAngle)
	{
		newAngle = newAngle / 180 * 3.14f;
		physicsBody->SetTransform(physicsBody->GetPosition(), newAngle);
	}
}

void Player::pickUpObject(PhysicsObject* objectToPickUp)
{
	objectToPickUp->PickedUp(this);
	heldObject = objectToPickUp;
	isHoldingObject = true;
}

void Player::throwObject()
{
	b2Vec2 dir = b2Vec2(heldObject->GetPhysicsBody()->GetPosition() - physicsBody->GetPosition());
	dir.Normalize();
	float power = throwPower * attackCharge;
	heldObject->Throw(power * dir);
	isHoldingObject = false;
}

void Player::punchPlayer(Player* enemyPlayer)
{
}

PhysicsObject* Player::getObjectInRange()
{
	PhysicsObject* objectToReturn = nullptr;
	float dist = 1000;	// Just a random large value which will be larger than any possible player catch range
	for each (PhysicsObject* obj in objectsInRange)
	{
		float tempDist = b2Vec2(obj->GetPhysicsBody()->GetPosition() - physicsBody->GetPosition()).LengthSquared();
		if (tempDist < dist)
		{
			objectToReturn = obj;
			dist = tempDist;
		}
	}

	return objectToReturn;
}

Player* Player::getPlayerInRange()
{
	Player* playerToReturn = nullptr;
	float dist = 1000;
	for each (Player* p in playersInRange)
	{
		float tempDist = b2Vec2(p->GetPhysicsBody()->GetPosition() - physicsBody->GetPosition()).LengthSquared();
		if (tempDist < dist)
		{
			playerToReturn = p;
			dist = tempDist;
		}
	}

	return playerToReturn;
}
