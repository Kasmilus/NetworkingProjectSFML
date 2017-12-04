#pragma once
#include "PhysicsObject.h"
#include "ClientCommandInput.h"
#include "ConnectionInfo.h"	// ClientActionCommand definition
#include <list>

class Player :
	public PhysicsObject
{
public:
	Player(b2World* physicsWorld, sf::Font* font, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~Player();

	void Update() override;	// Must be called on both server and client
	void UpdateControl();	// Called only on server
	void UpdateAnimation();
	void Hit();
	inline int IsHoldingObject() { return heldObject != nullptr ? heldObject->GetID() : -1; }	// Returns (-1) if no object is held
	inline void SetNetworkingVariables(float interp, float clockDifference) { currentInterp = interp; clientServerClockDifference = clockDifference; }

	// Server saving current player input after receiving it from player
	void SetPlayerInput(std::vector<ClientActionCommand>& commands);
	inline void SetOwningClient(sf::Uint8 id) { owningClientID = id; }
	inline sf::Uint8 GetOwningClientID() { return owningClientID; }

	// Collision calls
	void BeginCollision(b2Fixture* coll, bool isTrigger) override;
	void EndCollision(b2Fixture* coll, bool isTrigger) override;

	void ShowTaunt();

	bool ownedByClient;
	float tauntTextTimer;

	// Text
	sf::Text tauntText;

private:
	// Actions
	void move();
	void pickUpObject(PhysicsObject* objectToPickUp);
	void throwObject();
	void punchPlayer(Player* enemyPlayer);

	// Checks
	PhysicsObject* getObjectInRange();
	Player* getPlayerInRange();
	Player* getPlayerInRangeWithLagCompensation();

	// Variables
	bool isHoldingObject;
	float attackCharge;
	float throwPower;

	//
	b2Vec2 lastFramePos;
	float lastFrameRotation;

	// References
	PhysicsObject* heldObject;
	std::list<PhysicsObject*> objectsInRange;
	std::list<Player*> playersInRange;
	std::list<std::pair<float, Player*>> playerInRangeHistory;
	ClientCommandInput* playerInput;

	// Networking 
	sf::Uint8 owningClientID;
	float currentInterp;
	float clientServerClockDifference;

};

