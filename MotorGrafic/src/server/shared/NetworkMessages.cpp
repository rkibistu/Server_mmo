#include "NetworkMessages.h"
#include <magic_enum/magic_enum.hpp>

std::string NetworkMessages::PrepareMessage(NetworkTags tag, std::string content) {
    std::string tempMessage = ":" +  std::string(magic_enum::enum_name(tag)) + ":" + content;
    std::string message = std::to_string(tempMessage.length()) + tempMessage;

    return message;
}

void NetworkMessages::ParseMessage(std::string message, int& length, NetworkTags& tag, std::string& content) {
    size_t first_colon = message.find(":");
    size_t second_colon = message.find(":", first_colon + 1);

    length = std::stoi(message.substr(0, first_colon));

    std::string type = message.substr(first_colon + 1, second_colon - first_colon - 1);
    tag = FromString(type);

    content = message.substr(second_colon + 1);
}
