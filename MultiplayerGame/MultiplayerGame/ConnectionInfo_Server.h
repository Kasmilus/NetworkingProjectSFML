#pragma once
#include "ConnectionInfo.h"
#include <vector>
#include "ClientState.h"

/*
Class containing all info server needs
*/

class ConnectionInfo_Server :
	public ConnectionInfo
{
public:
	ConnectionInfo_Server();
	~ConnectionInfo_Server();

	inline void PassClientStatesPointer(std::vector<ClientState*>* clientStates) { this->clientStates = clientStates; }
	bool BindUDPSocket();
	bool ListenForConnections();
	void CloseConnectionWithClient(short clientID);

	// These functions are mean't to be overridden by server and player
	// ReceiverID - ID of the client who should receive the packet. Not relevant for client class(always sends to server)
	virtual bool SendPacketTCP(sf::Packet& packet, short receiverID = 0) override;
	virtual bool SendPacketUDP(sf::Packet& packet, short receiverID = 0) override;
	// Fills given packet object if received data
	virtual bool ReceivePacketTCP(sf::Packet& packet) override;
	virtual bool ReceivePacketUDP(sf::Packet& packet, short senderID = 0) override;

	// Create first packet to be sent after making connection
	sf::Packet CreateHandshakePacket(bool acceptClient);

private:
	// Server sockets
	sf::TcpListener myServerListenerTCP;
	sf::TcpSocket myServerSocketTCP;
	sf::UdpSocket myServerSocketUDP;

	// Listen
	sf::Socket::Status listenStatus;
	bool isListening;

	// Connected clients network info
	std::vector<sf::TcpSocket*> connectedClientSockets;

	// Connected players game info
	std::vector<ClientState*>* clientStates;

};

