#pragma once

#include <string>
#include "SFML/Network.hpp"
#include "Timer.h"
#include "Log.h"
#include "Box2D\Box2D.h"

/*
Class contains info clients and server need
It also provides basic interface for both of these classes

Functions will return false if failed(not necessarily an error, sockets are in non-blocking mode)
*/

// When connection is unconfirmed it means player is waiting for confirmation packet from the server right after making connection
enum ConnectionStatus { NoConnection, Disconnected, ConnectionUnconfirmed, ConnectionConfirmed };
// Commands - these would ideally be specified somewhere else with appropirate descriptions
enum ServerGameStateCommand : sf::Uint8 { Nothing, StartGame, EndRound, EndGame };
enum ServerUpdateType : sf::Uint8 { Position, Destroy, PlayerCharacter };	// This update contains Position - position update, Destroy if object should be destroyed, playerCharacter - it is character this client controls
enum ClientSimulationUpdate : sf::Uint8 { Nothingg, Death };	// Anything not related to player directly controlling character
enum ClientActionCommand : sf::Uint8 { MoveUp, MoveDown, MoveLeft, MoveRight, PressActionButton };

// Change float to short
const int CompactRange = 32766;
const int ExpandRange = 100;

static sf::Int16 CompactFloat(float input) {
	return sf::Int16(round(input * CompactRange / ExpandRange));
}
static float ExpandToFloat(sf::Int16 input) {
	return ((float)input) * ExpandRange / CompactRange;
}
// Interpolating between 2 values
static float Lerp(float a, float b, float t)
{
	return ((1 - t) * a) + (t * b);
}
// Cosine interpolation
static float CosInterp(float a, float b, float t)
{
	float t2 = (1 - cos(t*b2_pi)) / 2;
	return (a * (1 - t2) + b*t2);
}

// Client will send ClientPacket(and a number of ClientActionCommands) and receive ServerPacket(with a number of ServerUpdatePackets)
// Server will send ServerPacket(with a number of ServerUpdatePackets) and receive ClientPacket(and a number of ClientActionCommands)
struct ServerPacket
{
	float timestamp;
	sf::Uint32 lastReceivedUpdateID;	// Ideally should be just int16
	sf::Uint8 serverGameStateCommand;	// ServerGameStateCommand - Tell player any important information about game state
	sf::Uint8 numberOfUpdates;	// Tell client how many ServerUpdatePackets it contains
};
struct ServerUpdatePacket
{
	sf::Uint8 objectID;	// Each object in the game has an ID so server can easily correct every part of simulation on client machine
	sf::Uint8 updateType; // ServerUpdateType
	sf::Int16 positionX;	// Position - x 
	sf::Int16 positionY;	// Position - y
	bool boolField;	// Is it a player?(on game start)
	short shortField;	// Object size(on game start), object rotation after first packet
};
struct ClientPacket
{
	float timestamp;
	sf::Uint32 updateID;	// number of packet sent since game start
	sf::Uint8 objectID;	// ID of object ClientSimulationUpdate will refer to
	sf::Uint8 clientSimulationUpdate;	// ClientSimulationUpdate - That information is state of world from player's perspective so server can compare with it's own(used when conflicts arise)
	sf::Uint8 numberOfCommands;
};
struct ChatMessagePacket
{
	sf::Uint8 clientID;	// ID of the client who sent the message
	std::string PlayerName;
	std::string Message;
};


/*
Handshake:
Client asks server for connection(timestamp)
Server responds yes/no - so player can set simulation start time

Both sides will store current RTT and other side's local time as start time and then use that when receiving each message
(last received msg time - first received msg time = time since simulation start)

This could should be improved. E.g. sending another 2 messages so RTT could be measured better
*/
struct HandshakePacket
{
	bool accepted;	// Was connection accepted by the server? Not relevant for client side
	float timestamp;	// Real time when sending the message
	sf::Uint16 clientPort;	// Client sends port to which it's bound(UDP)
};

// Overload operators for easy (un)packing variables
// --- Handshake --- //
inline
sf::Packet& operator << (sf::Packet& packet, const HandshakePacket& handshake)
{
	return packet << handshake.accepted << handshake.timestamp << handshake.clientPort;
}
inline
sf::Packet& operator >> (sf::Packet& packet, HandshakePacket& handshake)
{
	return packet >> handshake.accepted >> handshake.timestamp >> handshake.clientPort;
}
// --- Server packets --- //
inline
sf::Packet& operator << (sf::Packet& packet, const ServerPacket& serverPacket)
{
	return packet << serverPacket.timestamp << serverPacket.lastReceivedUpdateID << serverPacket.serverGameStateCommand << serverPacket.numberOfUpdates;
}
inline
sf::Packet& operator >> (sf::Packet& packet, ServerPacket& serverPacket)
{
	return packet >> serverPacket.timestamp >> serverPacket.lastReceivedUpdateID >> serverPacket.serverGameStateCommand >> serverPacket.numberOfUpdates;
}
inline
sf::Packet& operator << (sf::Packet& packet, const ServerUpdatePacket& serverUpdatePacket)
{
	return packet << serverUpdatePacket.objectID << serverUpdatePacket.updateType << serverUpdatePacket.positionX << serverUpdatePacket.positionY << serverUpdatePacket.boolField << serverUpdatePacket.shortField;
}
inline
sf::Packet& operator >> (sf::Packet& packet, ServerUpdatePacket& serverUpdatePacket)
{
	return packet >> serverUpdatePacket.objectID >> serverUpdatePacket.updateType >> serverUpdatePacket.positionX >> serverUpdatePacket.positionY >> serverUpdatePacket.boolField >> serverUpdatePacket.shortField;
}
// --- Client packets --- //
inline
sf::Packet& operator << (sf::Packet& packet, const ClientPacket& clientPacket)
{
	return packet << clientPacket.timestamp << clientPacket.updateID << clientPacket.objectID << clientPacket.clientSimulationUpdate << clientPacket.numberOfCommands;
}
inline
sf::Packet& operator >> (sf::Packet& packet, ClientPacket& clientPacket)
{
	return packet >> clientPacket.timestamp >> clientPacket.updateID >> clientPacket.objectID >> clientPacket.clientSimulationUpdate >> clientPacket.numberOfCommands;
}
// --- Chat packets --- //
inline
sf::Packet& operator << (sf::Packet& packet, const ChatMessagePacket& chatMessage)
{
	return packet << chatMessage.clientID << chatMessage.PlayerName << chatMessage.Message;
}
inline
sf::Packet& operator >> (sf::Packet& packet, ChatMessagePacket& chatMessage)
{
	return packet >> chatMessage.clientID >> chatMessage.PlayerName >> chatMessage.Message;
}

class ConnectionInfo
{
public:
	ConnectionInfo();
	~ConnectionInfo();

	// These functions are meant to be overridden by server and player
	// ReceiverID - ID of the client who should receive the packet. Not relevant for client class(always sends to server)
	virtual bool SendPacketTCP(sf::Packet &packet, unsigned short receiverID = 0) = 0;
	virtual bool SendPacketUDP(sf::Packet &packet, unsigned short receiverID = 0) = 0;
	// Fills given packet object if received data
	virtual bool ReceivePacketTCP(sf::Packet& packet, unsigned short senderID = 0) = 0;
	virtual bool ReceivePacketUDP(sf::Packet& packet, unsigned short& senderID) = 0;

protected:
	// Returns true if status is unexpected error or socket disconnected
	bool CheckForError(sf::Socket::Status& status, std::string errorMsg);

protected:
	std::string serverIP;
	unsigned short serverPort;

};

