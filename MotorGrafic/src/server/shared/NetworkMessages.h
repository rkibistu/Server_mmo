#pragma once

#include <list>
#include <string>
#include <nlohmanJson/json.hpp>


enum NetworkTags {
	JoinGameRequest,
	JoinGameResponse,

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

	static void ParseMessage(std::string message, std::vector< NetworkPackage>& packages);

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

private:
	// a part of the previous recv buffer that was not compelte so was not parsed
	// it is used in the next parsing to combine and create a new message
	static std::string _remainingMessage;
	static bool _shouldComplete;
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

	JoinGameResponseData(int id) : Id(id) {}

	JoinGameResponseData(const std::string& serializedData) {
		nlohmann::json j = nlohmann::json::parse(serializedData);
		Id = j["Id"];
	}

	std::string Serialize() const {
		nlohmann::json j;
		j["Id"] = Id;
		return j.dump();
	}

	static JoinGameResponseData Deserialize(const std::string& data) {
		nlohmann::json j = nlohmann::json::parse(data);
		return { (int)j["Id"] };
	}
};
