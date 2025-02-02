#pragma once

#include <string>

#define SET_BIT(item, bit)      (item) |= (1 << (bit))
#define CLEAR_BIT(item, bit)    (item) &= ~(1 << (bit))
#define IS_BIT_SET(item, bit)   (((item) & (1 << (bit))) != 0)

class Utils {
public:
	static std::string readFile(const std::string& filePath);
	static float RandomFloat(float min, float max);
};
