#pragma once

#include "Parsers.hpp"
#include "Printers.hpp"
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
	static std::optional<::Color> parse(const std::string& str)
	{
		const auto props = Def::parse<std::unordered_map<std::string, std::string>>(str);
		if (not props)
			return std::nullopt;

		const auto rIt = props->find(Def::Color::R);
		const auto gIt = props->find(Def::Color::G);
		const auto bIt = props->find(Def::Color::B);
		const auto intensityIt = props->find(Def::Color::Intensity);

		if (rIt == props->end() || gIt == props->end() || bIt == props->end())
			return std::nullopt;

		const auto r = Def::parse<uint8_t>(rIt->second);
		const auto g = Def::parse<uint8_t>(gIt->second);
		const auto b = Def::parse<uint8_t>(bIt->second);
		const auto intensity = Def::parse<uint8_t>(intensityIt->second);

		if (not (r && g && b && intensity))
			return std::nullopt;

		return ::Color(*r, *g, *b, *intensity);
	}
};

template <>
class Def::Printer<Color>
{
public:
	static std::string print(const ::Color object)
	{
		std::unordered_map<std::string, uint8_t> props
		{
			{ Def::Color::R, object.r },
			{ Def::Color::G, object.g },
			{ Def::Color::B, object.b },
			{ Def::Color::Intensity, object.a }
		};

		return Def::print(props);
	}

	static std::string prettyPrint(const ::Color object)
	{
		std::unordered_map<std::string, uint8_t> props
		{
			{ Def::Color::R, object.r },
			{ Def::Color::G, object.g },
			{ Def::Color::B, object.b },
			{ Def::Color::Intensity, object.a }
		};

		return Def::prettyPrint(props);
	}
};
