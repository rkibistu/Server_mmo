#include "NetworkMessages.h"
#include <magic_enum/magic_enum.hpp>
#include <iostream>

#define DELIMITER "%"
#define START_DELIMITER "^"
#define END_DELIMITER "&"

std::string NetworkMessages::_remainingMessage = "";
bool NetworkMessages::_shouldComplete = false;

std::string NetworkMessages::PrepareMessage(NetworkTags tag, std::string content) {
	std::string tempMessage = DELIMITER + std::string(magic_enum::enum_name(tag)) + DELIMITER + content;
	
	std::string message = START_DELIMITER + std::to_string(tempMessage.length() + 2) + tempMessage + END_DELIMITER;

	return message;
}

void NetworkMessages::ParseMessage(std::string message, std::vector< NetworkPackage>& packages) {

	int length;
	NetworkTags tag;
	std::string content;

	while (true) {

		//std::cout << "------------------" << std::endl;

		size_t startDelimiter = message.find(START_DELIMITER);
		//std::cout << "startDel 1: " << startDelimiter << std::endl;
		if (startDelimiter != 0 || startDelimiter == std::string::npos) {
			if (_shouldComplete == false) {
				//We don t have the start of the message, so we drop it
				size_t endDelimiter = message.find(END_DELIMITER);
				//std::cout << "Should NOT complete! endDel: " << endDelimiter << std::endl;
				if (endDelimiter == std::string::npos) {
					
					throw "START and END delimiter are missing both. Treat this case too!!";
				}
				//std::cout << "Before: " << message << std::endl;
				message = message.substr(endDelimiter + 1);
				//std::cout << "After: " << message << std::endl;
			}
			else {
				//We have the start of the message. So let's rebuild it.
				//std::cout << "Should complete: " << message << std::endl;
				//std::cout << "Remained: " <<_remainingMessage << std::endl;
				message = _remainingMessage + message;
				_shouldComplete = false;
			}
		}
		startDelimiter = message.find(START_DELIMITER);
		//std::cout << "startDel 2: " << startDelimiter << std::endl;

		size_t first_colon = message.find(DELIMITER);
		if (first_colon == std::string::npos) {
			_remainingMessage = message;
			_shouldComplete = true;
			return; // Wait for more data
		}
		//std::cout << "first_colon: " << first_colon << std::endl;

		size_t second_colon = message.find(DELIMITER, first_colon + 1);
		if (second_colon == std::string::npos) {
			_remainingMessage = message;
			_shouldComplete = true;
			return; // Wait for more data
		}
		//std::cout << "second_colon: " << second_colon << std::endl;

		std::string lengthStr = message.substr(startDelimiter + 1, first_colon);
		length = std::stoi(lengthStr);

		//std::cout << "length: " << length << std::endl;

		//std::cout << "message: " << message << std::endl;

		// Check if the full message is available
		if (message.length() < length - lengthStr.length() + 1) {
			_remainingMessage = message;
			_shouldComplete = true;
			return;  // Wait for more data
		}

		std::string type = message.substr(first_colon + 1, second_colon - first_colon - 1);
		tag = FromString(type);

		std::cout << "tag: " << tag << std::endl;

		content = message.substr(second_colon + 1, length - second_colon);

		//std::cout << "content: " << content << std::endl;

		if (content[0] != '{' || content[content.length() - 1] != '}') {
			message = message.substr(1);
			continue;
		}

		packages.push_back({ length,tag,content });

		// Remove processed message from buffer
		message = message.substr(length + lengthStr.length() - 1);
		if (message.empty())
			return;
	}
}
