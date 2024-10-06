#pragma once

#include "Parsers.hpp"
#include "Printers.hpp"
#include "Keywords.hpp"

#include <cstdint>

enum class LabwareType : uint8_t
{
	NONE = 0,
	FLASK = 1,
	ADAPTOR = 2,
	CONDENSER = 3,
	FUNNEL = 4,
	SEP_FUNNEL = 5,
	DROP_FUNNEL = 6,
	HEATSOURCE = 7,
	STIRER = 8,
	HEATER_STIRRER = 9
};

static inline constexpr bool hasMultiLayerStorage(const LabwareType type)
{
	return 
		type == LabwareType::FLASK ||
		type == LabwareType::SEP_FUNNEL ||
		type == LabwareType::DROP_FUNNEL;
}

template <>
class Def::Parser<LabwareType>
{
public:
	static std::optional<LabwareType> parse(const std::string& str)
	{
		static const std::unordered_map<std::string, LabwareType> typeMap
		{
			{Keywords::Labware::Flask, LabwareType::FLASK},
			{Keywords::Labware::Adaptor, LabwareType::ADAPTOR},
			{Keywords::Labware::Condenser, LabwareType::CONDENSER},
			{Keywords::Labware::Heatsource, LabwareType::HEATSOURCE},
		};

		const auto typeIt = typeMap.find(Utils::strip(str));
		return typeIt != typeMap.end() ?
			std::optional(typeIt->second) :
			std::nullopt;
	}
};

template <>
class Def::Printer<LabwareType>
{
public:
	static std::string print(const LabwareType object)
	{
		static const std::unordered_map<LabwareType, std::string> typeMap
		{
			{LabwareType::FLASK, Keywords::Labware::Flask},
			{LabwareType::ADAPTOR, Keywords::Labware::Adaptor},
			{LabwareType::CONDENSER, Keywords::Labware::Condenser},
			{ LabwareType::HEATSOURCE, Keywords::Labware::Heatsource},
		};
		return typeMap.at(object);
	}
};