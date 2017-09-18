#include "PhysicsObject.h"



PhysicsObject::PhysicsObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
{
	this->physicsWorld = physicsWorld;

	b2BodyDef bodyDef;
	if (isDynamic)
		bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(posX , posY );
	physicsBody = physicsWorld->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(sizeX / 2 , sizeY / 2 );

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.0f;
	if (isDynamic)
		fixtureDef.density = 5.0f;
	fixtureDef.friction = 10.0f;
	physicsBody->CreateFixture(&fixtureDef);

	// Sprite

	sprite.setScale(sf::Vector2f((sizeX/2 / RATIO), (sizeY/2 / RATIO)));
}


PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::Update()
{
	updatePosition();
	clampVelocity();
}

void PhysicsObject::clampVelocity()
{
	b2Vec2 v = physicsBody->GetLinearVelocity();
	float maxV = 1;
	if (v.x > maxV)
		v.x = maxV;
	if (v.x < -maxV)
		v.x = -maxV;
	if (v.y > maxV)
		v.y = maxV;
	if (v.y < -maxV)
		v.y = -maxV;
	physicsBody->SetLinearVelocity(v);
}

void PhysicsObject::SetTexture(sf::Texture& texture)
{
	this->texture = &texture;
	sprite.setTexture(texture);
	sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().x / 2));
}

void PhysicsObject::updatePosition()
{
	sprite.setPosition(sf::Vector2f(physicsBody->GetPosition().x , physicsBody->GetPosition().y ));
	float angle = physicsBody->GetAngle() * RADTODEG;
	sprite.setRotation(angle);
}
