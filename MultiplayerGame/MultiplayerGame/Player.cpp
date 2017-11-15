#include "Player.h"
#include "SFML\System\Vector2.hpp"
#include <math.h>  


Player::Player(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
	:PhysicsObject(physicsWorld, isDynamic, posX, posY, sizeX, sizeY)
{
	// Add sensor fixture to detect objects to pick up, players to punch etc.
	b2CircleShape shape;
	shape.m_radius = sizeX * 1.25f;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.isSensor = true;
	physicsBody->CreateFixture(&fixtureDef);


	// Default values for variables
	isHoldingObject = false;
	throwPower = 1000;
	attackCharge = 0;
	canMove = false;

	heldObject = nullptr;
}


Player::~Player()
{
	if (heldObject)
	{
		heldObject->Drop();
		heldObject = 0;
	}
	objectsInRange.clear();
	playersInRange.clear();

	PhysicsObject::~PhysicsObject();
}

void Player::Update()
{
	if (canMove)
	{
		move();

		if (Input::Instance().IsSpaceDown())
		{
			Player* enemyPlayer = getPlayerInRange();
			if (enemyPlayer != nullptr)
			{
				punchPlayer(enemyPlayer);
			}
			if (enemyPlayer == nullptr)
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
		}
		else if (Input::Instance().IsSpaceReleased())
		{
			if (isHoldingObject)
			{
				throwObject();
			}
		}

		if (Input::Instance().IsSpacePressed())
		{
			// Charge attack
			attackCharge += Timer::Instance().GetDeltaTime();

			if (attackCharge > 3)
				attackCharge = 3;
		}
	}

	// Check if all pointers are valid
	for (std::list<PhysicsObject*>::const_iterator obj = objectsInRange.begin(); obj != objectsInRange.end(); )
	{
		if ((*obj)->IsMarkedForDestruction())
		{
			obj = objectsInRange.erase(obj);
		}
		else
		{
			++obj;
		}
	}
	for (std::list<Player*>::const_iterator player = playersInRange.begin(); player != playersInRange.end(); )
	{
		if ((*player)->IsMarkedForDestruction())
		{
			player = playersInRange.erase(player);
		}
		else
		{
			++player;
		}
	}

	PhysicsObject::Update();
}

void Player::Hit()
{
	Destroy();
}

void Player::BeginCollision(b2Fixture* coll, bool isTrigger)
{
	bool isDynamic = coll->GetBody()->GetType() == b2BodyType::b2_dynamicBody;
	PhysicsObject* collObj = static_cast<PhysicsObject*>(coll->GetBody()->GetUserData());
	Player* enemyPlayer = dynamic_cast<Player*>(collObj);

	// Something is in trigger range
	if (isTrigger && isDynamic)
	{
		// Enemy player which can be punched
		if (enemyPlayer)
		{
			playersInRange.push_back(enemyPlayer);
		}
		// Box which can be picked up
		if (collObj)
		{
			if (!enemyPlayer)
			{
				objectsInRange.push_back(collObj);
			}
		}
	}
	else if (!isTrigger)
	{
		// Hit by box
		if (collObj && !enemyPlayer && collObj != heldObject)
		{
			b2Vec2 objVelocity = collObj->GetPhysicsBody()->GetLinearVelocity();
			if (objVelocity.Length() * collObj->GetSize() > 0.75f)
			{
				// Check if object is flying in players direction
				b2Vec2 posDiff = physicsBody->GetPosition() - collObj->GetPhysicsBody()->GetPosition();
				if (b2Dot(objVelocity, posDiff) > 0.6f)
				{
					Hit();
				}
			}
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
	heldObject = nullptr;
	isHoldingObject = false;
}

void Player::punchPlayer(Player* enemyPlayer)
{
	enemyPlayer->Hit();
}

PhysicsObject* Player::getObjectInRange()
{
	PhysicsObject* objectToReturn = nullptr;
	float dist = 1000;	// Just a random large value which will be larger than any possible player catch range
	for each (PhysicsObject* obj in objectsInRange)
	{
		if (obj && obj->GetPhysicsBody())
		{
			float tempDist = b2Vec2(obj->GetPhysicsBody()->GetPosition() - physicsBody->GetPosition()).LengthSquared();
			if (tempDist < dist)
			{
				objectToReturn = obj;
				dist = tempDist;
			}
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
		if (p)
		{
			float tempDist = b2Vec2(p->GetPhysicsBody()->GetPosition() - physicsBody->GetPosition()).LengthSquared();
			if (tempDist < dist)
			{
				playerToReturn = p;
				dist = tempDist;
			}
		}
	}

	return playerToReturn;
}
