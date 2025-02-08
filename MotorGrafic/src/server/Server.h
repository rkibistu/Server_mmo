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
class DatabaseManager;

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
	void SendToAll(NetworkTags tag, std::string content);
	void HandleMessage(SOCKET clientSocket, std::string message);
	void HandleErrors(SOCKET clientSocket, int error);
	/**
	 * add to list
	 * send to all client the new client
	 * send to the new client all the existing clients
	 */
	void HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent);
	void HandleDisconnectClientRequest(SOCKET clientSocket, std::string messageContent);

	void DisconnectClient(SOCKET clientSocket);
	bool CheckLogin(std::string usenrame, std::string pass) { return true; }
	Player* HandleLogin(std::string username, std::string pass);

	// Create and add a new cleint to <socket,client> dictionary
	void AddNewClient(SOCKET fd);
	// Delete the associated Client and remove it from the _conIdClient too
	void RemoveClient(SOCKET fd);

	// just add to _conIdClient, don't create the client
	void AddNewClientToIdDict(int id, Client* client);
	// Jusr remove from _conIdClient, doesn't delete the Client
	void RemoveClientFromIdDIct(int id);

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
	// This one is opoulated oonyl after login
	// We need both ebcuase soemtimes we want to acces the client using the docket and 
	// sometimes usign the id (and we don t want loops everytime)
	std::unordered_map<int, Client*> _conIdClients;

	// HandleError method adds here clients that should be disconnected and removed
	// The method can't remove them becuase the method can be called inside a loop
	// and removing while looping triggers errors
	// So the method marks them and the program will remove them later in the loop
	std::vector<SOCKET> _clientToDisconnect;

	DatabaseManager* _dbManager;
	static int _clientIdsTemp;
};
