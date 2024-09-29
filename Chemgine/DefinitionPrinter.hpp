#pragma once

#include "Printers.hpp"
#include "Keywords.hpp"
#include "DefinitionObject.hpp"

template <>
class Def::Printer<DefinitionObject>
{
public:
	static std::string print(const DefinitionObject& object)
	{
		static const std::unordered_map<DefinitionType, std::string> typeMap
		{
			{DefinitionType::AUTO, Keywords::Types::Auto},
			{DefinitionType::DATA, Keywords::Types::Data},
			{DefinitionType::ATOM, Keywords::Types::Atom},
			{DefinitionType::RADICAL, Keywords::Types::Radical},
			{DefinitionType::RADICAL, Keywords::Types::Molecule},
			{DefinitionType::REACTION, Keywords::Types::Reaction},
			{DefinitionType::LABWARE, Keywords::Types::Labware},
		};

		std::string result = "_" + typeMap.at(object.getType());
		if (const auto identifier = object.getIdentifier(); identifier.size())
			result += '<' + identifier + '>';

		if (const auto specifier = object.getSpecifier(); specifier.size())
			result += ':' + specifier;

		if (const auto properties = object.getRemainingProperties(); properties.size())
		{
			result += '{';
			for (const auto& p : properties)
				result += p.first + ':' + p.second + ',';
			result.back() = '}';
		}

		result += ';';
		return result;
	}
};
