#pragma once
class Timer
{
public:
	static Timer& Instance();

	void Update(float deltaTime);

	float GetDeltaTime();

private:
	Timer() {}
	Timer(Timer const&);              // Don't Implement.
	void operator=(Timer const&); // Don't implement

	float deltaTime;

};

