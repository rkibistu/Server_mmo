#include "NetworkMessages.h"
#include <magic_enum/magic_enum.hpp>
#include <iostream>

#define DELIMITER "%"
#define START_DELIMITER "^"
#define END_DELIMITER "&"

std::string NetworkMessages::PrepareMessage(NetworkTags tag, std::string content) {
	std::string tempMessage = DELIMITER + std::string(magic_enum::enum_name(tag)) + DELIMITER + content;
	std::string message = START_DELIMITER + std::to_string(tempMessage.length() + 2) + tempMessage + END_DELIMITER;

	return message;
}

std::string NetworkMessages::ParseMessage(std::string remainingMessage, std::string message, std::vector< NetworkPackage>& packages) {

	int length;
	NetworkTags tag;
	std::string content;

	message = remainingMessage + message;
	while (true) {
		size_t startDelimiter = message.find(START_DELIMITER);
		if (startDelimiter != 0 || startDelimiter == std::string::npos) {
			// Start delimiter is missing, the message can't be decoded
			size_t endDelimiter = message.find(END_DELIMITER);
			if (endDelimiter == std::string::npos) {

				throw "START and END delimiter are missing both. Treat this case too!!";
			}

			// skip the message until the next start delimiter
			message = message.substr(endDelimiter + 1);
		}
		startDelimiter = message.find(START_DELIMITER);
		if (startDelimiter != 0) {
			throw "How? You just ckecked that info. It means you code checks it bad!";
		}

		size_t first_colon = message.find(DELIMITER);
		if (first_colon == std::string::npos) {
			return message; // Wait for more data
		}

		size_t second_colon = message.find(DELIMITER, first_colon + 1);
		if (second_colon == std::string::npos) {
			return message; // Wait for more data
		}

		std::string lengthStr = message.substr(startDelimiter + 1, first_colon - 1);
		length = std::stoi(lengthStr);

		// Check if the full message is available
		if (message.length() < length + lengthStr.length()) {
			return message;  // Wait for more data
		}

		std::string type = message.substr(first_colon + 1, second_colon - first_colon - 1);
		tag = FromString(type);

		content = message.substr(second_colon + 1, length + lengthStr.length() - (second_colon + 1) - 1);

		packages.push_back({ length,tag,content });

		// Remove processed message from buffer
		message = message.substr(length + lengthStr.length());
		if (message.empty())
			return message;
	}
}
