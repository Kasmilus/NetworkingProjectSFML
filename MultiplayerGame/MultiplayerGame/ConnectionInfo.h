#pragma once

#include <string>
#include "SFML/Network.hpp"
#include "Timer.h"
#include "Log.h"

/*
Class contains info both clients and server need
It also provides basic interface for both of these classes

Functions will return false if failed(not necessarily an error, sockets are in non-blocking mode)
*/

// When connection is unconfirmed it means player didn't hear back from server after sending first message
enum ConnectionStatus { NoConnection, Disconnected, ConnectionUnconfirmed, ConnectionConfirmed};

// Clients will send ClientPacket and receive ServerPacket
// Server will send ServerPacket and receive ClientPacket
struct ServerPacket
{

};
struct ClientPacket
{

};

/*
Handshake:
Client asks server for connection(timestamp)
Server responds yes/no - so player can set simulation start time

Both sides will store current RTT and other side's local time as start time and then use that when receiving each message
(last received msg time - first received msg time = time since simulation start)

This could should be improved. E.g. sending another 2 messages so both sides would know exact half round trip time(as it doesnt have to be RTT/2)
*/
struct HandshakePacket
{
	bool accepted;	// Was connection accepted by the server? Not relevant for client side
	float timestamp;	// Real time when sending the message
};

// Overload operators for easy (un)packing variables
inline
sf::Packet& operator <<(sf::Packet& packet, const HandshakePacket& handshake)
{
	return packet << handshake.accepted << handshake.timestamp;
}
inline
sf::Packet& operator >> (sf::Packet& packet, HandshakePacket& handshake)
{
	return packet >> handshake.accepted >> handshake.timestamp;
}
inline
sf::Packet& operator <<(sf::Packet& packet, const ServerPacket& serverPacket)
{
	return packet << 1;
}
inline
sf::Packet& operator >> (sf::Packet& packet, ServerPacket& serverPacket)
{
	return packet;
}
inline
sf::Packet& operator <<(sf::Packet& packet, const ClientPacket& clientPacket)
{
	return packet << 1;
}
inline
sf::Packet& operator >> (sf::Packet& packet, ClientPacket& clientPacket)
{
	return packet;
}

class ConnectionInfo
{
public:
	ConnectionInfo();
	~ConnectionInfo();

	// These functions are mean't to be overridden by server and player
	// ReceiverID - ID of the client who should receive the packet. Not relevant for client class(always sends to server)
	virtual bool SendPacketTCP(sf::Packet &packet, unsigned short receiverID = 0) = 0;
	virtual bool SendPacketUDP(sf::Packet &packet, unsigned short receiverID = 0) = 0;
	// Fills given packet object if received data
	virtual bool ReceivePacketTCP(sf::Packet& packet, unsigned short senderID = 0) = 0;
	virtual bool ReceivePacketUDP(sf::Packet& packet, unsigned short senderID = 0) = 0;

protected:
	// Returns true if status is unexpected error or socket disconnected
	bool CheckForError(sf::Socket::Status& status, std::string errorMsg);

protected:
	std::string serverIP;
	unsigned short serverPort;

};

