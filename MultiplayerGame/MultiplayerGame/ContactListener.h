#pragma once
#include "Box2D\Dynamics\b2WorldCallbacks.h"
#include "PhysicsObject.h"

class ContactListener :
	public b2ContactListener
{
public:
	ContactListener();
	~ContactListener();

	void BeginContact(b2Contact* contact);
	
	void EndContact(b2Contact* contact);
	
};

