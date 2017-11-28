#pragma once
#include "PhysicsObject.h"

class CrateObject : public PhysicsObject
{
public:
	CrateObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~CrateObject();

	void SetIsBeingHeld(bool value) { isBeingHeld = value; }

	// Collision calls
	virtual void BeginCollision(b2Fixture* coll, bool isTrigger) override;
	virtual void EndCollision(b2Fixture* coll, bool isTrigger) override;

private:
	bool isBeingHeld;
	int maxHealth;
	int health;

};

