#pragma once

#include <list>
#include <string>
#include <nlohmanJson/json.hpp>

#include "../Client.h"


enum NetworkTags {
	JoinGameRequest,
	JoinGameResponse,

	InfoConnectedClient,
	InfoConnectedClients,

	DisconnectClientRequest,
	DisconnectClientSignal,

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
		else if (tag == "InfoConnectedClient") {
			return NetworkTags::InfoConnectedClient;
		}
		else if (tag == "InfoConnectedClients") {
			return NetworkTags::InfoConnectedClients;
		}
		else if (tag == "DisconnectClientSignal") {
			return NetworkTags::DisconnectClientSignal;
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
			j["Clients"].push_back(nlohmann::json::parse(client.Serialize()));
		}
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