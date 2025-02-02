#pragma once

#include <WinSock2.h>
#include <vector>
#include <iostream>

#include <windows.h>


class Server{
public:
	static Server& GetInstance();
	// Close server
	void DestroyInstance();

	bool Init();
	void Close();

	/** Handles new conenctions/messagess (non blocinkg method) */
	void CheckIncomingTraffic();

private:
	void HandleMessage(std::string message);

private:
	static Server* _spInstance;
	Server() { ; }
	Server(const Server&) = delete;
	Server& operator= (const Server&) = delete;

	SOCKET _listenSocket;
	std::vector<WSAPOLLFD> _clients;
};
