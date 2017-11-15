#include "CrateObject.h"


CrateObject::CrateObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
	:PhysicsObject(physicsWorld, isDynamic, posX, posY, sizeX, sizeY)
{
	maxHealth = 3;
	health = maxHealth;
}

CrateObject::~CrateObject()
{
	PhysicsObject::~PhysicsObject();
}

void CrateObject::BeginCollision(b2Fixture* coll, bool isTrigger)
{
	float v = physicsBody->GetLinearVelocity().Length();
	float v2 = coll->GetBody()->GetLinearVelocity().Length();

	if (v > 0.6f || v2 > 0.6f)
	{
		health--;
		sprite.setColor(sf::Color(255 / maxHealth * health, 255 / maxHealth * health, 255 / maxHealth * health, 255));
		if (health <= 0)
		{
			Destroy();
		}
	}
}

void CrateObject::EndCollision(b2Fixture* coll, bool isTrigger)
{

}