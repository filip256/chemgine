#pragma once

#include "Parsers.hpp"
#include "Printers.hpp"

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

		if (const auto typeIt = typeMap.find(str); typeIt != typeMap.end())
			return typeIt->second;
		return std::nullopt;
	}
};

template <>
class Def::Printer<PortType>
{
public:
	static std::string print(const PortType object)
	{
		static const std::unordered_map<PortType, std::string> typeMap
		{
			{PortType::SIMPLE, "SIMPLE"},
			{PortType::INLET, "INLET"},
			{PortType::OUTLET, "OUTLET",},
			{PortType::DROPPER, "DROPPER"},
			{PortType::HOSE, "HOSE"},
			{PortType::CONTACT, "CONTACT"},
		};

		return typeMap.at(object);
	}
};