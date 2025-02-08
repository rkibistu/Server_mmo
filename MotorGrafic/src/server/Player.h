#pragma once

#include <string>
#include <utils/Math.h>

// This represents a loggedIn client
// This is something that exist (is spawned) only after a succesful login
// This is used to represent the client in the world 

class Player{
public:
	Player(int id, std::string username, rml::Vector3 position);

	int GetId() { return _id; }
	std::string GetUsername() { return _username; }

private:
	int _id;
	std::string _username;
	
	// a scene object maybe ?
};
