#pragma once

#include "PrintSettings.hpp"
#include "Printers.hpp"

#include <ostream>

class DataDumper
{
private:
	std::ostream& out;
	const Def::PrintSettings& settings;
	const uint8_t valueOffset;
	const std::string indent;

public:
	DataDumper(
		std::ostream& out,
		const uint8_t valueOffset,
		const uint16_t baseIndent,
		const bool prettify
	) noexcept;
	DataDumper(const DataDumper&) = default;

	template <typename T>
	DataDumper& header(
		const std::string& type,
		const T& specifier,
		const std::string& idendtifier);

	DataDumper& beginProperties();

	template <typename T>
	DataDumper& property(
		const std::string& name,
		const T& value);

	template <typename T>
	DataDumper& propertyWithSep(
		const std::string& name,
		const T& value,
		const bool preSep = false);

	DataDumper& endProperties();
	DataDumper& endDefinition();
};

template <typename T>
DataDumper& DataDumper::header(const std::string& type, const T& specifier, const std::string& idendtifier)
{
	out << '_' << type;
	if (idendtifier.size())
		out << '<' << idendtifier << '>';
	
	out << settings.specifierSep << (settings.prettify ? Def::prettyPrint(specifier) : Def::print(specifier));
	return *this;
}

template <typename T>
DataDumper& DataDumper::property(const std::string& name, const T& value)
{
	out << indent << name << ':';
	
	if(settings.prettify)
		out << std::string(valueOffset - name.size(), ' ') << Def::prettyPrint(value);
	else
		out << Def::print(value);

	return *this;
}

template <typename T>
DataDumper& DataDumper::propertyWithSep(const std::string& name, const T& value, const bool preSep)
{
	if (preSep)
		out << settings.propertySep;

	property(name, value);

	if(not preSep)
		out << settings.propertySep;

	return *this;
}
