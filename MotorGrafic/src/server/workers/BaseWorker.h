#pragma once

#include "server/Server.h"

class BaseWorker{
public:
	void HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent);
	void HandleDisconnectClientRequest(SOCKET clientSocket, std::string messageContent);
	void HandleMoveRequest(SOCKET clientSocket, std::string messageContent);

protected:
};
