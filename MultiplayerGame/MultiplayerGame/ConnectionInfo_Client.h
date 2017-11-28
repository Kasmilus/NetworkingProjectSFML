#pragma once
#include "ConnectionInfo.h"

/*
Class containing all info player needs
*/

class ConnectionInfo_Client :
	public ConnectionInfo
{
public:
	ConnectionInfo_Client();
	~ConnectionInfo_Client();

	bool BindUDPSocket();
	bool ConnectToServer();

	// These functions are mean't to be overridden by server and player
	// ReceiverID - ID of the client who should receive the packet. Not relevant for client class(always sends to server)
	virtual bool SendPacketTCP(sf::Packet &packet, short receiverID = 0) override;
	virtual bool SendPacketUDP(sf::Packet &packet, short receiverID = 0) override;
	// Fills given packet object if received data
	virtual bool ReceivePacketTCP(sf::Packet& packet) override;
	virtual bool ReceivePacketUDP(sf::Packet& packet, short senderID = 0) override;

	// Create first packet to be sent after making connection
	virtual sf::Packet CreateHandshakePacket();

	// Called when client received confirmation from the server
	inline void ConnectionWithServerAccepted() { connectionStatus = ConnectionStatus::ConnectionConfirmed; }
	void Disconnect();
	inline ConnectionStatus GetConnectionStatus() { return connectionStatus; }

private:
	ConnectionStatus connectionStatus;

	unsigned short MyClientPort = 53111;
	sf::UdpSocket MyClientSocketUDP;
	sf::TcpSocket MyClientSocketTCP;

};

