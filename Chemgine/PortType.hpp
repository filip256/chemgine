#pragma once

#include "Parsers.hpp"

#include <cstdint>

enum class PortType : uint8_t
{
	NONE = 0,
	SIMPLE = 1,
	INLET = 2,
	OUTLET = 3,
	DROPPER = 4,
	HOSE = 5,
	CONTACT = 6
};


template <>
class Def::Parser<PortType>
{
public:
	static std::optional<PortType> parse(const std::string& str)
	{
		static const std::unordered_map<std::string, PortType> typeMap
		{
			{"SIMPLE", PortType::SIMPLE},
			{"INLET", PortType::INLET},
			{"OUTLET", PortType::OUTLET},
			{"DROPPER", PortType::DROPPER},
			{"HOSE", PortType::HOSE},
			{"CONTACT", PortType::CONTACT},
		};

		const auto typeIt = typeMap.find(Utils::strip(str));
		return typeIt != typeMap.end() ?
			std::optional(typeIt->second) :
			std::nullopt;
	}
};
