#pragma once
#include "SFML/System/Clock.hpp"

class Timer
{
public:
	static Timer& Instance();

	void Update(float deltaTime);

	inline void SetClock(sf::Clock clock) { this->clock = &clock; }

	float GetDeltaTime();
	float GetSimulationTime();	// Returns time since game start(in seconds)

private:
	Timer() {}
	Timer(Timer const&);              // Don't Implement.
	void operator=(Timer const&); // Don't implement

	float deltaTime;
	sf::Clock* clock;
};

