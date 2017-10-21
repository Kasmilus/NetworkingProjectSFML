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

	// Getters/Setters
	void SetTexture(sf::Texture& texture);
	b2Body* GetPhysicsBody() { return physicsBody; }
	sf::Sprite* GetSprite() { return &sprite; }

	// Collision calls
	virtual void BeginCollision(b2Fixture* coll, bool isTrigger);
	virtual void EndCollision(b2Fixture* coll, bool isTrigger);

	

protected:
	// Physics
	b2World *physicsWorld;
	b2Body* physicsBody;
	b2RevoluteJoint* grabJoint;

	// Graphics
	sf::Texture* texture;
	sf::Sprite sprite;

	// Functions
	void updatePosition();
	void clampVelocity();

};

