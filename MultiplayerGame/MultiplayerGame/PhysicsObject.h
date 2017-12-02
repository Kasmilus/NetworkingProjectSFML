#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D\Box2D.h>
#include "Globals.h"
#include "Input.h"
#include "Timer.h"

class PhysicsObject
{
public:
	PhysicsObject();
	PhysicsObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~PhysicsObject();

	virtual void Update();
	void PickedUp(PhysicsObject* player);
	void Throw(b2Vec2 force);
	void Drop();

	// Getters/Setters
	void SetTexture(sf::Texture& texture);
	b2Body* GetPhysicsBody() { return physicsBody; }
	sf::Sprite* GetSprite() { return &sprite; }
	float GetSize() { return size; }
	inline bool IsMarkedForDestruction() { return isMarkedForDestruction; }
	inline void Destroy() { isMarkedForDestruction = true; }	// Will destroy the object at the end of the frame
	void SetID(sf::Uint8 value) { ID = value; }
	sf::Uint8 GetID() { return ID; }
	void SetAsServerObject() { isOnServer = true; }
	bool IsServerObject() { return isOnServer; }
	bool MovedSinceLastFrame() { return movedSinceLastFrame; }

	// Collision calls
	virtual void BeginCollision(b2Fixture* coll, bool isTrigger);
	virtual void EndCollision(b2Fixture* coll, bool isTrigger);

	

protected:
	//ID
	sf::Uint8 ID;

	// Physics
	b2World* physicsWorld;
	b2Body* physicsBody;
	b2RevoluteJoint* grabJoint;
	const float MAX_VELOCITY = 10.0f;
	b2Vec2 lastFramePos;

	// Graphics
	sf::Texture* texture;
	sf::Sprite sprite;

	// Other
	float size;
	bool isMarkedForDestruction;

	// Functions
	void updatePosition();
	void clampVelocity();

	// Networking
	bool isOnServer;	// if true - call all events as normally, otherwise just update state based on data received from the server
	bool movedSinceLastFrame;
};

