#pragma once
#include <WinSock2.h>
#include <string>

// A connected client
// A client is considered connected from the moemnt that
// the server accepted the connection; even a not logged in
// client is still a connected client
class Client {
public:
	Client(int id);

	int GetID() { return _id; }
	void SetLoggedIn(bool value) { _isLoggedIn = value; }
	void SetUsername(std::string username) { _username = username; }
private:
	int _id;
	std::string _username;
	bool _isLoggedIn = false;
};
