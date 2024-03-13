#pragma once

#include <cstdint>

class Color
{
public:
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 255;

	Color() = default;
	Color(const uint8_t red, const uint8_t green,
		const uint8_t blue, const uint8_t alpha
	) noexcept;
};
