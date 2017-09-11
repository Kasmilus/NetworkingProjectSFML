#include "PhysicsObject.h"



PhysicsObject::PhysicsObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
{
	this->physicsWorld = physicsWorld;

	b2BodyDef bodyDef;
	if (isDynamic)
		bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(posX, posY);
	physicsBody = physicsWorld->CreateBody(&bodyDef);	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(sizeX, sizeY);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.0f;
	if (isDynamic)
		fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	physicsBody->CreateFixture(&fixtureDef);
	// Sprite
	sprite.setScale(sf::Vector2f(sizeX, sizeY));
}


PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::Update()
{
	updatePosition();
}

void PhysicsObject::SetTexture(std::string filePath)
{
	texture.loadFromFile(filePath);
	sprite.setTexture(texture);
	sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().x / 2));
}

void PhysicsObject::updatePosition()
{
	sprite.setPosition(sf::Vector2f(physicsBody->GetPosition().x * PHYSICS_SCALE, physicsBody->GetPosition().y * PHYSICS_SCALE));
}
