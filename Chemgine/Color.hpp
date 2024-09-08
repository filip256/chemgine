#pragma once

#include "Parsers.hpp"

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


template <>
class Def::Parser<Color>
{
public:
	static std::optional<Color> parse(const std::string& str)
	{
		if (str.empty())
			return std::nullopt;

		const auto rgba = Def::parse<std::vector<uint8_t>>(str);
		if (rgba.has_value() == false || rgba->size() != 4)
			return std::nullopt;

		return Color((*rgba)[0], (*rgba)[1], (*rgba)[2], (*rgba)[3]);
	}
};
