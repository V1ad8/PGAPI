#pragma once

#include "utils/glm_utils.h"
#include <string>

namespace color
{
    // Translate color from hexa to rgb
	inline glm::vec3 Translate(const std::string &hexa)
	{
		std::string hex = hexa;

		// Remove '#' if present
		while (hex[0] == '#') {
			hex.erase(0, 1);
		}

		// Handle shorthand hex (#FFF -> FFFFFF)
		if (hex.length() == 3) {
			std::string expanded;
			for (char c : hex)
				expanded.append(2, c); // duplicate each char
			hex = expanded;
		}

		// Must be 6 characters (RRGGBB)
		if (hex.length() != 6) {
			return glm::vec3(1, 1, 1);
		}

		// Parse each color component
		unsigned int r = std::stoi(hex.substr(0, 2), nullptr, 16);
		unsigned int g = std::stoi(hex.substr(2, 2), nullptr, 16);
		unsigned int b = std::stoi(hex.substr(4, 2), nullptr, 16);

		// Normalize to [0, 1]
		return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
	}

}   // namespace color
