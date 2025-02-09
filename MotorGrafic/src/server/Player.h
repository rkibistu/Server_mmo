#pragma once

#include <string>
#include <utils/Math.h>

class Mesh;

struct BoundingVolume {
	rml::Vector3 Min;
	rml::Vector3 Max;
};

// This represents a loggedIn client
// This is something that exist (is spawned) only after a succesful login
// This is used to represent the client in the world 
class Player{
public:
	Player(int id, std::string username, rml::Vector3 position);

	int GetId() { return _id; }
	std::string GetUsername() { return _username; }

	rml::Vector3 GetPosition() { return _position; }
	void SetPosition(rml::Vector3& pos) { _position = pos; }

private:
	void Spawn();
	void GenerateBoundingVolume(Mesh* mesh);

private:
	int _id;
	std::string _username;
	
	// All i need to know is the position and the bounding volume
	// Movement is based on movement from client
	// So i don t need velocity or other things
	// And no visuals on server
	// On client we can use a sceneobject with the same mesh. There we need a scene
	// object so we can render it
	rml::Vector3 _position;
	BoundingVolume _boundingVolume;
};
