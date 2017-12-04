#include "ClientState.h"


ClientState::ClientState(std::string ip)
{
	this->ip = ip;
	points = 0;
	gameWinner = false;
	lastRoundTripTime = 0;
	connectionStatus = ConnectionStatus::ConnectionUnconfirmed;
}

ClientState::~ClientState()
{
}
