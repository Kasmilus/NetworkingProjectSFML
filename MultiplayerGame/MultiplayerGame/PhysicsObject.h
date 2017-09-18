#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D\Box2D.h>
#include "Globals.h"
#include "Input.h"
#include "Timer.h"

class PhysicsObject
{
public:
	PhysicsObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~PhysicsObject();

	virtual void Update();

	void SetTexture(sf::Texture& texture);

	sf::Sprite* GetSprite() { return &sprite; }

protected:
	// Physics
	b2World *physicsWorld;
	b2Body* physicsBody;

	// Graphics
	sf::Texture* texture;
	sf::Sprite sprite;

	//
	void updatePosition();
	void clampVelocity();
};

