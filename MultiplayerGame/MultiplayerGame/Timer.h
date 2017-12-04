#pragma once
#include "SFML/System/Clock.hpp"

class Timer
{
public:
	static Timer& Instance();

	void Update(float deltaTime);

	float GetDeltaTime();
	float GetSimulationTime();	// Returns time since game start(in seconds)

private:
	Timer() {}
	Timer(Timer const&);         
	void operator=(Timer const&);

	float timeSinceStart = 0.0f;
	float deltaTime;
};

