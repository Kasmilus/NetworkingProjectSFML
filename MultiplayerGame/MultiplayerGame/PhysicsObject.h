#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D\Box2D.h>


class PhysicsObject
{
public:
	PhysicsObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY);
	~PhysicsObject();

	virtual void Update();

	void SetTexture(std::string filePath);

	sf::Sprite* GetSprite() { return &sprite; }

private:
	// Physics
	b2World *physicsWorld;
	b2Body* physicsBody;

	const float PHYSICS_SCALE = 20.0f;

	// Graphics
	sf::Texture texture;
	sf::Sprite sprite;

	void updatePosition();
};

