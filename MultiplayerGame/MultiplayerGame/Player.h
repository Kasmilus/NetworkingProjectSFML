#pragma once
#include "PhysicsObject.h"
#include <list>

class Player :
	public PhysicsObject
{
public:
	Player(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~Player();

	void Update() override;

	// Collision calls
	void BeginCollision(b2Fixture* coll, bool isTrigger) override;
	void EndCollision(b2Fixture* coll, bool isTrigger) override;

private:
	// Actions
	void move();
	void pickUpObject(PhysicsObject* objectToPickUp);
	void throwObject();
	void punchPlayer(Player* enemyPlayer);

	// Checks
	PhysicsObject* getObjectInRange();
	Player* getPlayerInRange();
	
	// Variables
	bool isHoldingObject;
	float attackCharge;
	float throwPower;

	// References
	PhysicsObject* heldObject;
	std::list<PhysicsObject*> objectsInRange;
	std::list<Player*> playersInRange;


};

