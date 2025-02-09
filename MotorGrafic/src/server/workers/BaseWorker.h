#pragma once

#include "server/Server.h"

class BaseWorker{
public:
	void HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent);
	void HandleDisconnectClientRequest(SOCKET clientSocket, std::string messageContent);
	void HandleMoveRequest(SOCKET clientSocket, std::string messageContent);

	// This should be called at the end of the frame
	// Check if the movement resulted in collisions and rollback 
	// the movement where it is necessary
	virtual void ResolveMovement() = 0;

protected:
	int _collisionMarker[MAX_CONNECTIONS];
};
