#include "Timer.h"


Timer& Timer::Instance()
{
	static Timer instance;
	return instance;
}

void Timer::Update(float deltaTime)
{
	this->deltaTime = deltaTime;
}

float Timer::GetDeltaTime()
{
	return deltaTime;
}