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
