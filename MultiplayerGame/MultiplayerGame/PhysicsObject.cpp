#include "PhysicsObject.h"



PhysicsObject::PhysicsObject()
{
	// Should never be called, for some reason compiler doesn't like it not having empty constructor(because of static_cast in ContactListener?)
}

PhysicsObject::PhysicsObject(b2World* physicsWorld, bool isDynamic, float posX, float posY, float sizeX, float sizeY)
{
	this->physicsWorld = physicsWorld;

	// Body definition
	b2BodyDef bodyDef;
	if (isDynamic)
		bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(posX, posY);
	bodyDef.angularDamping = 0.2f;
	bodyDef.linearDamping = 0.015f + (sizeX/100);
	physicsBody = physicsWorld->CreateBody(&bodyDef);
	// Shape
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(sizeX / 2, sizeY / 2);
	// Fixture
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.0f;
	if (isDynamic)
		fixtureDef.density = 1.0f + (sizeX/5);
	fixtureDef.friction = 1.0f;
	fixtureDef.restitution = 1;
	physicsBody->CreateFixture(&fixtureDef);

	// Sprite
	sprite.setScale(sf::Vector2f((sizeX / 2 / RATIO), (sizeY / 2 / RATIO)));

	// User data
	physicsBody->SetUserData(this);
}


PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::Update()
{
	updatePosition();
	clampVelocity();
}

void PhysicsObject::PickedUp(PhysicsObject* player)
{
	physicsBody->SetLinearVelocity(b2Vec2_zero);

	// Set position in front of the player
	float rotation = player->GetPhysicsBody()->GetAngle();
	b2Vec2 offset = b2Vec2(cos(rotation), sin(rotation));	// make vector from the rotation
	offset *= 5;

	physicsBody->SetTransform(player->GetPhysicsBody()->GetPosition() + offset, physicsBody->GetAngle());

	// Setup joint
	b2RevoluteJointDef jointDef;
	jointDef.Initialize(player->GetPhysicsBody(), physicsBody, physicsBody->GetPosition());
	jointDef.collideConnected = true;
	jointDef.lowerAngle = 0;
	jointDef.upperAngle = 0;
	jointDef.enableLimit = true;
	grabJoint = (b2RevoluteJoint*)physicsWorld->CreateJoint(&jointDef);


}

void PhysicsObject::Throw(b2Vec2 force)
{
	physicsWorld->DestroyJoint(grabJoint);
	physicsBody->ApplyForceToCenter(force, true);
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
	sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
	sf::Vector2f x = sprite.getOrigin();
}

void PhysicsObject::BeginCollision(b2Fixture * coll, bool isTrigger)
{
}

void PhysicsObject::EndCollision(b2Fixture * coll, bool isTrigger)
{
}

void PhysicsObject::updatePosition()
{
	// Update sprite
	sprite.setPosition(sf::Vector2f(physicsBody->GetPosition().x, physicsBody->GetPosition().y));
	float angle = physicsBody->GetAngle() * 180 / 3.14f;
	sprite.setRotation(angle);
}
