#pragma once

#include "Parsers.hpp"
#include "Printers.hpp"

enum class EstimationMode
{
	CONSTANT,
	LINEAR,
	QUADRATIC,
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
			{"QUADRATIC", EstimationMode::QUADRATIC},
			{"CUBIC", EstimationMode::CUBIC},
		};

		return Utils::find(typeMap, str);
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
			{EstimationMode::QUADRATIC, "QUADRATIC"},
			{EstimationMode::CUBIC, "CUBIC"},
		};

		return typeMap.at(object);
	}
};
