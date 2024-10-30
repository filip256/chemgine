#pragma once

#include "Parsers.hpp"
#include "Printers.hpp"

enum class EstimationMode
{
	CONSTANT,
	LINEAR,
	QUADRIATIC,
	CUBIC
};


template <>
class Def::Parser<EstimationMode>
{
public:
	static std::optional<EstimationMode> parse(const std::string& str)
	{
		static const std::unordered_map<std::string, EstimationMode> typeMap
		{
			{"CONSTANT", EstimationMode::CONSTANT},
			{"LINEAR", EstimationMode::LINEAR},
			{"QUADRIATIC", EstimationMode::QUADRIATIC},
			{"CUBIC", EstimationMode::CUBIC},
		};

		const auto typeIt = typeMap.find(Utils::strip(str));
		return typeIt != typeMap.end() ?
			std::optional(typeIt->second) :
			std::nullopt;
	}
};

template <>
class Def::Printer<EstimationMode>
{
public:
	static std::string print(const EstimationMode object)
	{
		static const std::unordered_map<EstimationMode, std::string> typeMap
		{
			{EstimationMode::CONSTANT, "CONSTANT"},
			{EstimationMode::LINEAR, "LINEAR"},
			{EstimationMode::QUADRIATIC, "QUADRIATIC"},
			{EstimationMode::CUBIC, "CUBIC"},
		};
		return typeMap.at(object);
	}
};
