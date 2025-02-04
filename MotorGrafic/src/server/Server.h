#pragma once

#pragma comment (lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <windows.h>
#include <ws2tcpip.h>

#include "server/shared/NetworkMessages.h"

class Client;

class Server {
public:
	static Server& GetInstance();
	// Close server
	void DestroyInstance();

	bool Init();
	void Close();

	Client* GetClient(SOCKET fd) { return _conClients[fd]; }

	/** Handles new conenctions/messagess (non blocinkg method) */
	void CheckIncomingTraffic();

private:
	void Send(SOCKET clientSocket, NetworkTags tag, std::string content);
	void SendToAllExcept(SOCKET clientSocket, NetworkTags tag, std::string content);
	void HandleMessage(SOCKET clientSocket, std::string message);

	/**
	 * add to list
	 * send to all client the new client
	 * send to the new client all the existing clients
	 */
	void HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent);
	void HandleDisconnectClientRequest(SOCKET clientSocket, std::string messageContent);

	void DisconnectClient(SOCKET clientSocket);
	bool CheckLogin(std::string usenrame, std::string pass) { return true; }

	void AddNewClient(SOCKET fd);
	void RemoveClient(SOCKET fd);

private:
	static Server* _spInstance;
	Server() { ; }
	Server(const Server&) = delete;
	Server& operator= (const Server&) = delete;


	SOCKET _listenSocket;
	// We keep both structeres of data here just for easier iterations
	// when reading incoming traffic
	std::vector<WSAPOLLFD> _clients;
	std::unordered_map<SOCKET, Client*> _conClients;

	static int _clientIdsTemp;
};
