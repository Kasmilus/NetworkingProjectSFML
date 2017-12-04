#pragma once

#include <string>
#include "ConnectionInfo.h"

/*
Client persistent state 
It keeps information associated with client with given IP rather than object
So that information stays even after player was killed/disconnected
*/

class ClientState
{
public:
	ClientState(std::string ip);
	~ClientState();

	// Connection related info
	inline std::string GetIpAddress() { return ip; }
	inline ConnectionStatus GetConnectionStatus() { return connectionStatus; }
	inline void SetConnected() { connectionStatus = ConnectionStatus::ConnectionConfirmed; }
	inline void SetDisconnected() { connectionStatus = ConnectionStatus::NoConnection; }
	inline void SetSimulationStartTime(float t) { simulationStartTime = t; }
	inline float GetSimulationStartTime() { return simulationStartTime; }
	inline void SetRTT(float value) { lastRoundTripTime = value; }
	inline float GetRTT() { return lastRoundTripTime; }

	// Game related info
	inline void AddPoint() { points++; }
	inline void ResetPoints() { points = 0; }
	inline int GetPoints() { return points; }
	inline void SetGameWinner(bool value) { gameWinner = value; }
	inline bool IsGameWinner() { return gameWinner; }

private:
	// Connection related info
	std::string ip;
	ConnectionStatus connectionStatus;
	float simulationStartTime;
	float lastRoundTripTime;

	// Game related info
	int points;
	bool gameWinner;
};

