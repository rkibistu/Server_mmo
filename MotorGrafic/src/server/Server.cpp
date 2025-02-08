#include "Server.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <magic_enum/magic_enum.hpp>

#include "Client.h"
#include "DatabaseManager.h"


#define DEFAULT_PORT "12345"
#define DEFAULT_BUFLEN 1024

Server* Server::_spInstance = nullptr;
int Server::_clientIdsTemp = 0;

Server& Server::GetInstance() {
	if (_spInstance == nullptr)
		_spInstance = new Server();
	return *_spInstance;
}

void Server::DestroyInstance() {

	Close();

	if (_spInstance)
		delete _spInstance;
}

bool Server::Init() {

	WSADATA wsaData;
	int iResult;

	_listenSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(_listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(_listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}

	_clients.push_back({ _listenSocket, POLLRDNORM, 0 });

	_dbManager = new DatabaseManager();
	// Insert a test user
	//_dbManager->insertUser("testUser", "testPass", 1.1, 2.2, 3.3);


	std::cout << "Server listening on port " << DEFAULT_PORT << "...\n";
}

void Server::Close() {

	closesocket(_listenSocket);
	WSACleanup();
}

void Server::CheckIncomingTraffic() {
	// Last parameter of wsapoll here (0) tells him to return immediatley.
	// It handles new messages if there are any, otherwise return so we can 
	// continue the server loop
	if (WSAPoll(_clients.data(), _clients.size(), 0) > 0) {
		for (size_t i = 0; i < _clients.size(); ++i) {
			if (_clients[i].revents & POLLRDNORM) {
				if (_clients[i].fd == _listenSocket) {
					// Accept new client
					SOCKET new_client = accept(_listenSocket, NULL, NULL);

					std::cout << "New client connected: " << new_client << "\n";
					_clients.push_back({ new_client, POLLRDNORM, 0 });
					AddNewClient(new_client);
					std::cout << "Sunt " << _conClients.size() << " conectati\n";

				}
				else {
					// Handle client data
					char buffer[DEFAULT_BUFLEN];
					int bytes_received = recv(_clients[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes_received <= 0) {
						std::cout << "Client disconnected: " << _clients[i].fd << "\n";
						DisconnectClient(_clients[i].fd);
						closesocket(_clients[i].fd);
						_clients.erase(_clients.begin() + i);
					}
					else {
						buffer[bytes_received] = '\0';
						std::cout << "%%%%\n";
						std::cout << buffer << std::endl;
						std::cout << "%%%%\n";
						HandleMessage(_clients[i].fd, buffer);
					}
				}
			}
		}
	}

	if (_clientToDisconnect.size() > 0) {
		for (auto client : _clientToDisconnect) {
			if (_conClients.find(client) != _conClients.begin()) {
				if (_conClients[client]) {

					DisconnectClientData data(_conClients[client]->GetID());
					RemoveClient(client);
					SendToAll(NetworkTags::DisconnectClientSignal, data.Serialize());
				}
				else {
					RemoveClient(client);
				}
			}
			else {
				RemoveClient(client);
			}
		}
		_clientToDisconnect.clear();
		std::cout << "Dupa deconect mai Sunt " << _conClients.size() << " conectati\n";
	}
}


void Server::Send(SOCKET clientSocket, NetworkTags tag, std::string content) {
	std::string message = NetworkMessages::PrepareMessage(tag, content);
	int totalSent = 0;
	int messageLength = message.length();
	while (totalSent < messageLength) {
		int bytesSent = send(clientSocket, message.c_str() + totalSent, messageLength - totalSent, 0);

		if (bytesSent == SOCKET_ERROR) {
			int errorCode = WSAGetLastError();
			std::cerr << "Send failed with error: " << errorCode << std::endl;
			HandleErrors(clientSocket, errorCode);
			return;
		}

		totalSent += bytesSent;
	}

	std::cout << "SENT: " << content << std::endl;
}

void Server::SendToAllExcept(SOCKET clientSocket, NetworkTags tag, std::string content) {
	for (auto client : _conClients) {
		if (client.first != clientSocket && client.first != _listenSocket) {
			Send(client.first, tag, content);
		}
	}
}

void Server::SendToAll(NetworkTags tag, std::string content) {
	for (auto client : _conClients) {
		if (client.first != _listenSocket) {
			Send(client.first, tag, content);
		}
	}
}

void Server::HandleMessage(SOCKET clientSocket, std::string message) {
	std::vector<NetworkPackage> packages;

	std::string remainingMessage = NetworkMessages::ParseMessage(_conClients[clientSocket]->RemainingMessage, message, packages);
	_conClients[clientSocket]->RemainingMessage = remainingMessage;

	for (auto package : packages) {
		std::cout << "[" + std::string(magic_enum::enum_name(package.Tag)) + "]: " + package.Content + "\n";
		switch (package.Tag) {
		case JoinGameRequest:
			HandleJoinGameRequest(clientSocket, package.Content);
			break;
		case DisconnectClientRequest:
			HandleDisconnectClientRequest(clientSocket, package.Content);
		default:
			break;
		}
	}

}

void Server::HandleErrors(SOCKET clientSocket, int error) {
	switch (error) {
	case WSAECONNRESET:
	{
		//Connection reset by peer. An existing connection was forcibly closed by the remote host.
		// This normally results if the peer application on the remote host is suddenly stopped, 
		// the host is rebooted, the host or remote network interface is disabled, or the remote host
		// uses a hard close
		std::cout << "Error " << error << " handles. Disconetcted client: " << clientSocket << "\n";
		if (_conClients.find(clientSocket) != _conClients.end()) {
			_clientToDisconnect.push_back(clientSocket);
		}
		break;
	}
	default:
		break;
	}
}

void Server::HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent) {

	JoinGameRequestData data(messageContent);

	// we use -1 to mark a failed login
	JoinGameResponseData response(-1);

	Player* player = HandleLogin(data.Username, data.Password);
	if (player == nullptr) {
		Send(clientSocket, NetworkTags::JoinGameResponse, response.Serialize());
		return;
	}

	Client* client = GetClient(clientSocket);
	client->SetPlayer(player);


	// trimite informatiile despre noul client tuturor celorlalti
	InfoConnectedClientData info{ client->GetID(), client->GetUsername() };
	SendToAllExcept(clientSocket, InfoConnectedClient, info.Serialize());

	//trimite raspuns la cel care a initiat logarea
	response.Id = client->GetID();
	Send(clientSocket, NetworkTags::JoinGameResponse, response.Serialize());

	//trimite info clientului nou despre toti clientii conectati
	std::vector<InfoConnectedClientData> temp;
	for (auto client : _conClients) {
		if (client.first != clientSocket)
			temp.push_back({ client.second->GetID(), client.second->GetUsername() });
	}
	InfoConnectedClientsData allConClients(temp);
	Send(clientSocket, NetworkTags::InfoConnectedClients, allConClients.Serialize());
}

void Server::HandleDisconnectClientRequest(SOCKET clientSocket, std::string messageContent) {
	DisconnectClient(clientSocket);
}

void Server::DisconnectClient(SOCKET clientSocket) {
	if (_conClients.find(clientSocket) == _conClients.end()) {
		std::cout << "[ERROR]: Tried to disconnect a client that didn;t exist in the dictionary. Check it!\n";
		return;
	}

	DisconnectClientData data(_conClients[clientSocket]->GetID());
	SendToAll(NetworkTags::DisconnectClientSignal, data.Serialize());
	RemoveClient(clientSocket);
}

Player* Server::HandleLogin(std::string username, std::string pass) {

	// Select and display all users
	Player* player;
	User user = _dbManager->selectUser(username);
	if (user.exist == true) {
		// already connected username OR wrong password -> no login
		if (_conClients.find(user.Id) != _conClients.end() || user.Password != pass)
			return nullptr;
	}
	else {
		// we should create the user
		user.Username = username;
		user.Password = pass;
		user.PosX = 0;
		user.PosY = 0;
		user.PosZ = 0;

		bool okay = _dbManager->insertUser(user);
		if (okay == false)
			return nullptr;
	}

	player = new Player(user.Id, user.Username, rml::Vector3(user.PosX, user.PosY, user.PosZ));
	return player;
}

void Server::AddNewClient(SOCKET fd) {
	if (_conClients.find(fd) != _conClients.end()) {
		throw "A client with the same fd already is connecteed. This should never happen.";
	}
	Client* client = new Client(_clientIdsTemp++);
	_conClients[fd] = client;
}

void Server::RemoveClient(SOCKET fd) {
	auto it = _conClients.find(fd);
	if (it != _conClients.end()) {
		delete it->second;  // Delete the dynamically allocated Client
		it->second = nullptr;
	}
	_conClients.erase(fd);

	for (auto it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->fd == fd) {
			_clients.erase(it);
			break; // Erasing invalidates the iterator, so exit loop
		}
	}
}
