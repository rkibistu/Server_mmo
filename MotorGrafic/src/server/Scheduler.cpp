#include "Scheduler.h"

#include "Server.h"

void Scheduler::Add(SOCKET clientSocket, NetworkPackage package) {
	_packages.push({ clientSocket, package });
}

void Scheduler::Resolve() {

	Server& server = Server::GetInstance();

	while (_packages.empty() == false) {

		ScheudelerPackage package = _packages.front();
		_packages.pop();

		switch (package.Package.Tag) {
		case JoinGameRequest:
			server.HandleJoinGameRequest(package.ClientSocker, package.Package.Content);
			break;
		case DisconnectClientRequest:
			server.HandleDisconnectClientRequest(package.ClientSocker, package.Package.Content);
			break;
		case MoveRequest:
			server.HandleMoveRequest(package.ClientSocker, package.Package.Content);
			break;
		default:
			break;
		}
	}
}
