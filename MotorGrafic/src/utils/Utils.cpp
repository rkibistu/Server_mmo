#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include "Utils.h"

#include <core/MemoryDebug.h>

std::string Utils::readFile(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cout << "[UTILS][ERROR] Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();  // Read the file's buffer into the stream
    shaderFile.close();
    return shaderStream.str();  // Return the content as a string
}

float Utils::RandomFloat(float min, float max) {
    // Create a random number generator and a distribution
    std::random_device rd;                     // Seed generator
    std::mt19937 gen(rd());                    // Standard Mersenne Twister generator
    std::uniform_real_distribution<float> dist(min, max); // Distribution from min to max

    return dist(gen); // Generate and return random float in [min, max]
}
