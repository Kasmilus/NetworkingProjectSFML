#include "ContactListener.h"



ContactListener::ContactListener()
{
}


ContactListener::~ContactListener()
{
}

void ContactListener::BeginContact(b2Contact * contact)
{
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();
	// Call collision function in body A
	void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData)
		static_cast<PhysicsObject*>(bodyUserData)->BeginCollision(fixB, (fixA->IsSensor() || fixB->IsSensor()));

	// Call collision function in body B
	bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData)
		static_cast<PhysicsObject*>(bodyUserData)->BeginCollision(fixA, (fixA->IsSensor() || fixB->IsSensor()));

}

void ContactListener::EndContact(b2Contact * contact)
{
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();

	// Call collision function in body A
	void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData)
		static_cast<PhysicsObject*>(bodyUserData)->EndCollision(fixB, (fixA->IsSensor() || fixB->IsSensor()));

	// Call collision function in body B
	bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData)
		static_cast<PhysicsObject*>(bodyUserData)->EndCollision(fixA, (fixA->IsSensor() || fixB->IsSensor()));

}
