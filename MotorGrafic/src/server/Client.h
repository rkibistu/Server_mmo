#pragma once
#include <WinSock2.h>
#include <string>

#include "Player.h"
// A connected client
// A client is considered connected from the moemnt that
// the server accepted the connection; even a not logged in
// client is still a connected client
class Client {
public:
	Client(SOCKET fd);
	~Client();

	int GetID() { return (_player == nullptr) ? -1 : _player->GetId(); }
	std::string GetUsername() { return (_player == nullptr) ? "-1" : _player->GetUsername(); }

	Player* GetPlayer() { return _player; }
	void SetPlayer(Player* player) { _player = player; }

public:
	// Important variable used to decode messages. 
	// It is a part from the previous message got by this client
	// that coudn't be encoded yet because it was not complete
	std::string RemainingMessage = "";
private:
	SOCKET _fd;
	// it exist only after login, reppresent a logged in client
	Player* _player = nullptr;
};
