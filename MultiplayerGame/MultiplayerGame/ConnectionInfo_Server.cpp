#include "ConnectionInfo_Server.h"



ConnectionInfo_Server::ConnectionInfo_Server()
{
	myServerSocketUDP.setBlocking(false);
	myServerListenerTCP.setBlocking(false);
	isListening = false;
}


ConnectionInfo_Server::~ConnectionInfo_Server()
{
	/*for (auto socket : connectedClientSockets)
	{
		socket->disconnect();
		delete socket;
	}*/
	connectedClientSockets.clear();

	myServerSocketUDP.unbind();
	myServerListenerTCP.close();
}

bool ConnectionInfo_Server::BindUDPSocket()
{
	sf::Socket::Status status = myServerSocketUDP.bind(serverPort);
	if (status != sf::Socket::Done)
	{
		LOG(ERROR) << "Socket binding error.";
		return false;
	}

	return true;
}

bool ConnectionInfo_Server::ListenForConnections()
{
	if(!isListening)
		listenStatus = myServerListenerTCP.listen(serverPort);

	if (listenStatus == sf::Socket::Done)
	{
		isListening = true;

		// accept a new connection
		connectedClientSockets.push_back(make_unique<sf::TcpSocket>());
		std::unique_ptr<sf::TcpSocket>& client = connectedClientSockets.back();
		sf::Socket::Status acceptStatus = myServerListenerTCP.accept(*client);
		if (acceptStatus == sf::Socket::Done)
		{
			// Save that client socket
			client->setBlocking(false);
			// Save client info
			ClientState* clientState = new ClientState(client->getRemoteAddress().toString());
			clientStates->push_back(clientState);

			LOG(INFO) << "Client with ID " << connectedClientSockets.size()-1 << " connected.";
			return true;
		}
		else
		{
			connectedClientSockets.pop_back();
		}

		if (acceptStatus != sf::Socket::NotReady)
		{
			LOG(ERROR) << "Error connecting to client. ID: " << connectedClientSockets.size() << ".";
		}
		return false;
	}
	else
	{
		if (listenStatus != sf::Socket::NotReady)
		{
			LOG(ERROR) << "Error when listening for incoming connections.";
			return false;
		}
	}

	return true;
}

bool ConnectionInfo_Server::CheckIfClientDisconnected(sf::Socket::Status status, unsigned short clientID)
{
	if (status == sf::Socket::Status::Disconnected)
	{
		clientStates->at(clientID)->SetDisconnected();
		connectedClientSockets.erase(connectedClientSockets.begin() + clientID);
		LOG(WARNING) << "Client closed connection.";
		return true;
	}

	return false;
}

void ConnectionInfo_Server::CloseConnectionWithClient(unsigned short clientID)
{
	connectedClientSockets.at(clientID)->disconnect();
}

bool ConnectionInfo_Server::SendPacketTCP(sf::Packet& packet, unsigned short receiverID)
{
	// Check if ID is correct
	if (receiverID >= connectedClientSockets.size())
	{
		LOG(ERROR) << "Client with that ID isn't connected! (invalid receiverID: " << receiverID << ").";
		return false;
	}
	else if (receiverID < 0)
	{
		LOG(ERROR) << "Client ID can't be negati! (invalid receiverID: " << receiverID << ").";
		return false;
	}

	// Send packet
	sf::Socket::Status status = connectedClientSockets.at(receiverID)->send(packet);
	bool error = CheckForError(status, "Server send packet (TCP) error.");
	if (error)
		return false;
	if (status != sf::Socket::Status::Done)
		return false;

	return true;
}

bool ConnectionInfo_Server::SendPacketUDP(sf::Packet& packet, unsigned short receiverID)
{
	// Check if ID is correct
	if (receiverID >= connectedClientSockets.size())
	{
		LOG(ERROR) << "Client with that ID isn't connected! (invalid receiverID: " << receiverID << ").";
		return false;
	}
	else if (receiverID < 0)
	{
		LOG(ERROR) << "Client ID can't be negative! (invalid receiverID: " << receiverID << ").";
		return false;
	}

	// Get client's IP and port
	sf::IpAddress ip = connectedClientSockets.at(receiverID)->getRemoteAddress();
	unsigned short port = connectedClientSockets.at(receiverID)->getRemotePort();

	// Send packet
	sf::Socket::Status status = myServerSocketUDP.send(packet, ip, port);
	bool error = CheckForError(status, "Server send packet (UDP) error.");
	if (error)
		return false;
	if (status != sf::Socket::Status::Done)
		return false;

	return true;
}

bool ConnectionInfo_Server::ReceivePacketTCP(sf::Packet& packet, unsigned short senderID)
{
	sf::Socket::Status status = connectedClientSockets.at(senderID)->receive(packet);
	bool error = CheckForError(status, "Server receive packet (TCP) error.");
	if (error)
		return false;

	if (status == sf::Socket::Done)
	{
		// Packet is filled with data, return true so calling function knows it can unpack it
		return true;
	}

	return false;
}

bool ConnectionInfo_Server::ReceivePacketUDP(sf::Packet& packet, unsigned short senderID)
{
	// Check if ID is correct
	if (senderID >= connectedClientSockets.size())
	{
		LOG(ERROR) << "Client with that ID isn't connected! (invalid senderID: " << senderID << ".";
		return false;
	}
	else if (senderID < 0)
	{
		LOG(ERROR) << "Client ID can't be negative! (invalid senderID: " << senderID << ".";
		return false;
	}

	// Get client's IP and port
	sf::IpAddress ip = connectedClientSockets.at(senderID)->getRemoteAddress();
	unsigned short port = connectedClientSockets.at(senderID)->getRemotePort();

	sf::Socket::Status status = myServerSocketUDP.receive(packet, ip, port);
	bool error = CheckForError(status, "Server receive packet (UDP) error.");
	if (error)
		return false;

	if (status == sf::Socket::Done)
	{
		// Packet is filled with data, return true so calling function knows it can unpack it
		return true;
	}

	return false;
}

sf::Packet ConnectionInfo_Server::CreateHandshakePacket(bool acceptClient)
{
	sf::Packet packet;

	// Create variable to send
	HandshakePacket packetInfo;
	packetInfo.accepted = acceptClient;
	packetInfo.timestamp = Timer::Instance().GetSimulationTime();

	// Write into the packet
	packet << packetInfo;

	return packet;
}
