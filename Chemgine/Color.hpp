#pragma once

#include "Parsers.hpp"
#include "Keywords.hpp"

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
		const auto props = Def::parse<std::unordered_map<std::string, std::string>>(str);
		if (not props.has_value())
			return std::nullopt;

		const auto rIt = props->find(Keywords::Color::R);
		const auto gIt = props->find(Keywords::Color::G);
		const auto bIt = props->find(Keywords::Color::B);
		const auto intensityIt = props->find(Keywords::Color::Intensity);

		if (rIt == props->end() || gIt == props->end(); bIt == props->end())
			return std::nullopt;

		const auto r = Def::parse<uint8_t>(rIt->second);
		const auto g = Def::parse<uint8_t>(gIt->second);
		const auto b = Def::parse<uint8_t>(bIt->second);
		const auto intensity = Def::parse<uint8_t>(intensityIt->second);

		if (not (r.has_value() && g.has_value() && b.has_value() && intensity.has_value()))
			return std::nullopt;

		return Color(*r, *g, *b, *intensity);
	}
};
