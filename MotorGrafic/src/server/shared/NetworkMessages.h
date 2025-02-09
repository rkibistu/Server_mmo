#pragma once

#include <list>
#include <string>
#include <iostream>
#include <nlohmanJson/json.hpp>
#include <utils/Math.h>
#include <magic_enum/magic_enum.hpp>

#include "../Client.h"

#define MAX_MOVEMENT_MAGNITUDE 0.01
#define MAX_CONNECTIONS 1000

enum NetworkTags {
	JoinGameRequest, // login
	JoinGameResponse,// succes/ failed (Id == -1 means failed)  + position in thw worled here

	InfoConnectedClient, //send by the server to all cleints when a new client connects
	InfoConnectedClients, // send to the new client, info about all the already connected client

	DisconnectClientRequest, // a client announce that he is disconnecting
	DisconnectClientSignal, // the server signals all the cleints about the disconnected one

	MoveRequest, // a clients signal his desire to move
	MoveResponse, // send the positions of all players that moved since last sent. Or, sent all position if the client is new

	Invalid
};

struct NetworkPackage {
	int Length; // length without the bytes of the length. Everything after length
	NetworkTags Tag;
	std::string Content;
};

class NetworkMessages {
public:

	static std::string PrepareMessage(NetworkTags tag, std::string content);

	/**
	 * It gets the message .
	 *
	 * \param remainingMessage the value returned last time when the method was called
	 * \param message the actual message that will be concatenated with first param and decoded
	 * \param packages out value, all decoded packages
	 * \return the message that containt an incomplete package that is need in the next call of the method
	 */
	static std::string ParseMessage(std::string remainingMessage, std::string message, std::vector< NetworkPackage>& packages);

	static NetworkTags FromString(std::string tag) {
		auto enumValue = magic_enum::enum_cast<NetworkTags>(tag); // Try to cast the string to the enum
		if (enumValue.has_value()) {
			return enumValue.value(); // Return the corresponding enum value
		}
		else {
			return NetworkTags::Invalid; // Return Invalid if the tag doesn't match any enum value
		}
	}
};


struct JoinGameRequestData {

	std::string Username;
	std::string Password;

	JoinGameRequestData(std::string username, std::string password) {
		Username = username;
		Password = password;
	}
	JoinGameRequestData(const std::string& serialisedData) {
		nlohmann::json j = nlohmann::json::parse(serialisedData);
		Username = j["Username"];
		Password = j["Password"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["Username"] = Username;
		j["Password"] = Password;
		return j.dump();
	}

	static JoinGameRequestData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { j["Username"], j["Password"] };
	}
};

struct JoinGameResponseData {
	int Id;
	rml::Vector3 Pos;

	JoinGameResponseData(int id, rml::Vector3 pos = rml::Vector3(0,0,0)) : Id(id), Pos(pos) {}

	JoinGameResponseData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Id = j["Id"];
		Pos.x = j["X"];
		Pos.y = j["Y"];
		Pos.z = j["Z"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["Id"] = Id;
		j["X"] = Pos.x;
		j["Y"] = Pos.y;
		j["Z"] = Pos.z;
		return j.dump();
	}

	static JoinGameResponseData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { (int)j["Id"], rml::Vector3(j["X"],j["Y"],j["Z"]) };
	}
};

struct InfoConnectedClientData {
	int Id;
	std::string Username;

	InfoConnectedClientData(int id, std::string username) : Id(id), Username(username) {}

	InfoConnectedClientData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Id = j["Id"];
		Username = j["Username"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["Id"] = Id;
		j["Username"] = Username;
		return j.dump();
	}

	static InfoConnectedClientData Deserialize(const std::string& data) {

		nlohmann::json j = nlohmann::json::parse(data);
		return { (int)j["Id"], j["Username"] };
	}
};

struct InfoConnectedClientsData {
	std::vector<InfoConnectedClientData> Clients;

	InfoConnectedClientsData() = default;

	InfoConnectedClientsData(std::vector<InfoConnectedClientData> clients)
		: Clients(std::move(clients)) {
	}

	InfoConnectedClientsData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		for (const auto& item : j["Clients"]) {
			Clients.emplace_back(item.dump());
		}
	}

	// Serialize the entire list to JSON string
	std::string Serialize() const {
		nlohmann::json j;
		j["Clients"] = nlohmann::json::array();
		for (const auto& client : Clients) {
			j["Clients"].push_back({ {"Id", client.Id}, {"Username", client.Username} });
		}
		std::cout << j.dump() << std::endl;
		return j.dump();
	}

	// Static function to deserialize from a JSON string
	static InfoConnectedClientsData Deserialize(const std::string& data) {
		return InfoConnectedClientsData(data);
	}
};

struct DisconnectClientData {
	int Id;

	DisconnectClientData(int id) : Id(id) {}

	DisconnectClientData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Id = j["Id"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["Id"] = Id;
		return j.dump();
	}

	static DisconnectClientData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { (int)j["Id"] };
	}
};

struct MoveData {
	int Id;
	rml::Vector3 Pos;
	rml::Vector3 Movement; // This is the vector movement used to move the player
							//it is not send over the networked, sued itnernally for rolleback movements

	MoveData(int id, rml::Vector3 pos, rml::Vector3 movement = rml::Vector3(0,0,0)) : Id(id), Pos(pos) {}

	MoveData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Id = j["Id"];
		Pos.x = j["X"];
		Pos.y = j["Y"];
		Pos.z = j["Z"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["Id"] = Id;
		j["X"] = Pos.x;
		j["Y"] = Pos.y;
		j["Z"] = Pos.z;
		return j.dump();
	}

	static MoveData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { (int)j["Id"], rml::Vector3(j["X"], j["Y"], j["Z"]) };
	}
};
struct MoveRequestData {
	rml::Vector3 Movement; // represent the movement vector

	MoveRequestData(rml::Vector3 movement) : Movement(movement) {}

	MoveRequestData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Movement.x = j["X"];
		Movement.y = j["Y"];
		Movement.z = j["Z"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["X"] = Movement.x;
		j["Y"] = Movement.y;
		j["Z"] = Movement.z;
		return j.dump();
	}

	static MoveRequestData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { rml::Vector3(j["X"], j["Y"], j["Z"]) };
	}
};
struct MoveResponseData {
	std::vector<MoveData> MovedPlayers;

	MoveResponseData() = default;

	MoveResponseData(std::vector<MoveData> movedPlayers)
		: MovedPlayers(std::move(movedPlayers)) {
	}

	MoveResponseData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		for (const auto& item : j["Players"]) {
			MovedPlayers.emplace_back(item.dump());
		}
	}

	// Serialize the entire list to JSON string
	std::string Serialize() const {
		nlohmann::json j;
		for (const auto& player : MovedPlayers) {
			j["Players"].push_back(nlohmann::json::parse(player.Serialize()));
		}
		return j.dump();
	}

	// Static function to deserialize from a JSON string
	static MoveResponseData Deserialize(const std::string& data) {
		return MoveResponseData(data);
	}
};

//nlohmann::json j;
//j["Players"] = nlohmann::json::array();
//for (const auto& client : MovedPlayers) {
//	j["Players"].push_back({ {"Id", client.Id}, {j["X"] = client.Pos.x} ,{j["Y"] = client.Pos.y},{ j["Z"] = client.Pos.z } });
//}
//std::cout << j.dump() << std::endl;
//return j.dump();