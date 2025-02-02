#pragma once

#include <list>
#include <string>
#include <nlohmanJson/json.hpp>


enum NetworkTags {
	JoinGameRequest,
	JoinGameResponse,

	Invalid
};

class NetworkMessages {
public:

	static std::string PrepareMessage(NetworkTags tag, std::string content);

	static void ParseMessage(std::string message, int& length, NetworkTags& tag, std::string& content);

	static NetworkTags FromString(std::string tag) {
		if (tag == "JoinGameRequest") {
			return NetworkTags::JoinGameRequest;
		}
		else if (tag == "JoinGameResponse") {
			return NetworkTags::JoinGameResponse;
		}
		else {
			return NetworkTags::Invalid;
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

	// Constructor
	JoinGameResponseData(int id) : Id(id) {}

	// Constructor for deserialization
	JoinGameResponseData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Id = j["Id"];
	}

	// Serialize the object to a JSON string
	std::string Serialize() const {
		nlohmann::json j;
		j["Id"] = Id;
		return j.dump();
	}

	// Deserialize a JSON string to an object
	static JoinGameResponseData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { (int)j["Id"] };
	}
};
