#include "Server.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <magic_enum/magic_enum.hpp>

#include "Client.h"



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
				}
				else {
					// Handle client data
					char buffer[DEFAULT_BUFLEN];
					int bytes_received = recv(_clients[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes_received <= 0) {
						std::cout << "Client disconnected: " << _clients[i].fd << "\n";
						RemoveClient(_clients[i].fd);
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
}


void Server::Send(SOCKET clientSocket, NetworkTags tag, std::string content) {
	std::string message = NetworkMessages::PrepareMessage(tag, content);
	send(clientSocket, message.c_str(), message.length(), 0);
}

void Server::HandleMessage(SOCKET clientSocket, std::string message) {
	std::vector<NetworkPackage> packages;
	NetworkMessages::ParseMessage(message, packages);

	for (auto package : packages) {
		std::cout << "[" + std::string(magic_enum::enum_name(package.Tag)) + "]: " + package.Content + "\n";
		switch (package.Tag) {
		case JoinGameRequest:
			HandleJoinGameRequest(clientSocket, package.Content);
			break;
		default:
			break;
		}
	}

}

void Server::HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent) {

	JoinGameRequestData data(messageContent);

	// we use -1 to mark a failed login
	JoinGameResponseData response(-1);

	if (!CheckLogin(data.Username, data.Password)) {
		//Send(clientSocket, NetworkTags::JoinGameResponse, response.Serialize());
		return;
	}

	Client* client = GetClient(clientSocket);
	client->SetLoggedIn(true);
	client->SetUsername(data.Username);

	response.Id = client->GetID();
	//Send(clientSocket, NetworkTags::JoinGameResponse, response.Serialize());
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
	}
	_conClients.erase(fd);
}
