#include "ConnectionInfo_Client.h"



ConnectionInfo_Client::ConnectionInfo_Client()
{
	MyClientSocketUDP.setBlocking(false);
	MyClientSocketTCP.setBlocking(false);
	connectionStatus = ConnectionStatus::NoConnection;
}


ConnectionInfo_Client::~ConnectionInfo_Client()
{
	MyClientSocketTCP.disconnect();
	MyClientSocketUDP.unbind();
}

bool ConnectionInfo_Client::BindUDPSocket()
{
	sf::Socket::Status status = MyClientSocketUDP.bind(sf::Socket::AnyPort);
	if (status != sf::Socket::Done)
	{
		LOG(ERROR) << "Socket binding error.";
		return false;
	}

	LOG(INFO) << "Client UDP socket bound to port " << MyClientSocketUDP.getLocalPort();
	return true;
}

bool ConnectionInfo_Client::ConnectToServer()
{
	sf::Socket::Status status = MyClientSocketTCP.connect(serverIP, serverPort);
	if (status != sf::Socket::Done)
	{
		if (status != sf::Socket::NotReady)
		{
			LOG(ERROR) << "Client attempt to connect with server failed.";
		}
		return false;
	}

	connectionStatus = ConnectionStatus::ConnectionUnconfirmed;
	LOG(INFO) << "Connected with server.";

	return true;
}

bool ConnectionInfo_Client::CheckIfDisconnected(sf::Socket::Status status)
{
	if (status == sf::Socket::Status::Disconnected)
	{
		connectionStatus = ConnectionStatus::Disconnected;
		MyClientSocketUDP.unbind();
		LOG(WARNING) << "Server closed connection.";
		return true;
	}

	return false;
}

bool ConnectionInfo_Client::SendPacketTCP(sf::Packet& packet, unsigned short receiverID)
{
	sf::Socket::Status status = MyClientSocketTCP.send(packet);
	bool connectionBroken = CheckIfDisconnected(status);
	if (connectionBroken)
		return false;
	bool error = CheckForError(status, "Client send packet (TCP) error.");
	if (error)
		return false;
	if (status != sf::Socket::Status::Done)
		return false;

	return true;
}

bool ConnectionInfo_Client::SendPacketUDP(sf::Packet& packet, unsigned short receiverID)
{
	sf::Socket::Status status = MyClientSocketUDP.send(packet, serverIP, serverPort);
	bool connectionBroken = connectionStatus == ConnectionStatus::Disconnected;	// UDP doesn't have to be connected but if there's no TCP connection, server won't receive UDP either
	if (connectionBroken)
		return false;
	bool error = CheckForError(status, "Client send packet (UDP) error.");
	if (error)
		return false;
	if (status != sf::Socket::Status::Done)
		return false;

	return true;
}

bool ConnectionInfo_Client::ReceivePacketTCP(sf::Packet& packet, unsigned short senderID)
{
	sf::Socket::Status status = MyClientSocketTCP.receive(packet);
	bool connectionBroken = CheckIfDisconnected(status);
	if (connectionBroken)
		return false;
	bool error = CheckForError(status, "Client receive packet (TCP) error.");
	if (error)
		return false;

	if (status == sf::Socket::Done)
	{
		// Packet is filled with data, return true so calling function knows it can unpack it
		return true;
	}

	return false;
}

bool ConnectionInfo_Client::ReceivePacketUDP(sf::Packet& packet, unsigned short& senderID)
{
	sf::IpAddress tempIP;
	unsigned short tempPort;
	sf::Socket::Status status = MyClientSocketUDP.receive(packet, sf::IpAddress(tempIP), tempPort);
	bool connectionBroken = CheckIfDisconnected(status);	// UDP doesn't have to be connected but if there's no TCP connection, server won't receive UDP either
	if (connectionBroken)
		return false;
	bool error = CheckForError(status, "Client receive packet (UDP) error.");
	if (error)
		return false;

	if (status == sf::Socket::Done)
	{
		// Packet is filled with data, return true so calling function knows it can unpack it
		return true;
	}

	return false;
}

sf::Packet ConnectionInfo_Client::CreateHandshakePacket()
{
	sf::Packet packet;

	// Create variable to send
	HandshakePacket packetInfo;
	packetInfo.accepted = true;
	packetInfo.timestamp = Timer::Instance().GetSimulationTime();
	packetInfo.clientPort = MyClientSocketUDP.getLocalPort();

	// Write into the packet
	packet << packetInfo;

	return packet;
}

void ConnectionInfo_Client::Disconnect()
{
	if (connectionStatus != ConnectionStatus::NoConnection)
	{
		MyClientSocketTCP.disconnect();
		LOG(INFO) << "Disconnected from the server.";
		connectionStatus = ConnectionStatus::NoConnection;
	}
}
