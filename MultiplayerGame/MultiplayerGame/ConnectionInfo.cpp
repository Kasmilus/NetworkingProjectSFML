#include "ConnectionInfo.h"



ConnectionInfo::ConnectionInfo()
{
	serverIP = "127.0.0.1";
	serverPort = 5312;
}


ConnectionInfo::~ConnectionInfo()
{
}

bool ConnectionInfo::CheckForError(sf::Socket::Status & status, std::string errorMsg)
{
	if (status != sf::Socket::Done)
	{
		if (status != sf::Socket::NotReady)
		{
			// Isn't done and not ready so it's some kind of error
			LOG(ERROR) << errorMsg;

			if (status == sf::Socket::Partial)
			{
				LOG(ERROR) << " - Sent only part of the packet, needs to be resend.";
			}
			return true;
		}
	}
	return false;
}
