#pragma once
#include "ConnectionInfo.h"
#include <vector>
#include <memory>
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
	inline int GetConnectedClientsNumber() { return connectedClientSockets.size(); }
	bool BindUDPSocket();
	bool ListenForConnections();
	void CloseConnectionWithClient(unsigned short clientID);
	bool CheckIfClientDisconnected(sf::Socket::Status status, unsigned short clientID);
	void SaveClientUDPAddress(unsigned short clientID, sf::Uint16 port);

	// These functions are mean't to be overridden by server and player
	// ReceiverID - ID of the client who should receive the packet. Not relevant for client class(always sends to server)
	virtual bool SendPacketTCP(sf::Packet& packet, unsigned short receiverID) override;
	virtual bool SendPacketUDP(sf::Packet& packet, unsigned short receiverID) override;
	// Fills given packet object if received data
	virtual bool ReceivePacketTCP(sf::Packet& packet, unsigned short senderID) override;
	virtual bool ReceivePacketUDP(sf::Packet& packet, unsigned short& senderID) override;

	// Create first packet to be sent after making connection
	sf::Packet CreateHandshakePacket(bool acceptClient);

private:
	// Server sockets
	sf::TcpListener myServerListenerTCP;
	sf::UdpSocket myServerSocketUDP;

	// Listen
	sf::Socket::Status listenStatus;
	bool isListening;

	// Connected clients network info
	std::vector<std::unique_ptr<sf::TcpSocket>> connectedClientSockets;
	std::vector<sf::Uint16> connectedClientSocketsUDP;	// just ports, testing it on local machine anyway

	// Connected players game info
	std::vector<ClientState*>* clientStates;

};

