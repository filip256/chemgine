#pragma once

#include "PrintSettings.hpp"
#include "Printers.hpp"
#include "UnitizedEstimator.hpp"
#include "MetaUtils.hpp"

#include <ostream>

namespace Def
{
	class DataDumper
	{
	private:
		std::ostream& out;
		const Def::PrintSettings& settings;
		const uint8_t valueOffset;
		std::string baseIndent;

	public:
		DataDumper(
			std::ostream& out,
			const uint8_t valueOffset,
			const uint16_t baseIndent,
			const bool prettify
		) noexcept;
		DataDumper(const DataDumper&) = default;
		DataDumper(DataDumper&&) = default;

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

		template <typename T>
		DataDumper& defaultProperty(
			const std::string& name,
			const T& value,
			const T& defaultValue);

		template <typename T>
		DataDumper& defaultPropertyWithSep(
			const std::string& name,
			const T& value,
			const T& defaultValue,
			const bool preSep = false);

		template <typename T, typename HistoryT>
		DataDumper& tryOolSubDefinition(
			const T& value,
			HistoryT& alreadyPrinted);

		template <typename T, typename HistoryT>
		DataDumper& subDefinition(
			const std::string& name,
			const T& value,
			HistoryT& alreadyPrinted);

		template <typename T, typename HistoryT>
		DataDumper& subDefinitionWithSep(
			const std::string& name,
			const T& value,
			HistoryT& alreadyPrinted,
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
		out << baseIndent << settings.propertyIndent << name << ':';

		if (settings.prettify)
			out << std::string(valueOffset - name.size(), ' ') << Def::prettyPrint(value);
		else
			out << Def::print(value);

		return *this;
	}

	template <typename T>
	DataDumper& DataDumper::propertyWithSep(const std::string& name, const T& value, const bool preSep)
	{
		if (preSep)
			out << settings.propertySep << settings.newLine;

		property(name, value);

		if (not preSep)
			out << settings.propertySep << settings.newLine;

		return *this;
	}

	template <typename T>
	DataDumper& DataDumper::defaultProperty(
		const std::string& name,
		const T& value,
		const T& defaultValue)
	{
		if (not Utils::equal(value, defaultValue))
			property(name, value);
		return *this;
	}

	template <typename T>
	DataDumper& DataDumper::defaultPropertyWithSep(
		const std::string& name,
		const T& value,
		const T& defaultValue,
		const bool preSep)
	{
		if (not Utils::equal(value, defaultValue))
			propertyWithSep(name, value, preSep);
		return *this;
	}

	template <typename T, typename HistoryT>
	DataDumper& DataDumper::tryOolSubDefinition(
		const T& value,
		HistoryT& alreadyPrinted)
	{
		value->dumpDefinition(out, settings.prettify, alreadyPrinted, false, 0);
		return *this;
	}

	template <typename T, typename HistoryT>
	DataDumper& DataDumper::subDefinition(
		const std::string& name,
		const T& value,
		HistoryT& alreadyPrinted)
	{
		out << baseIndent << settings.propertyIndent << name << ':';

		if (settings.prettify)
			out << std::string(valueOffset - name.size(), ' ');
		value->dumpDefinition(out, settings.prettify, alreadyPrinted, true, valueOffset + settings.propertyIndent.size() + 1);

		return *this;
	}

	template <typename T, typename HistoryT>
	DataDumper& DataDumper::subDefinitionWithSep(
		const std::string& name,
		const T& value,
		HistoryT& alreadyPrinted,
		const bool preSep)
	{
		if (preSep)
			out << settings.propertySep << settings.newLine;

		subDefinition(name, value, alreadyPrinted);

		if (not preSep)
			out << settings.propertySep << settings.newLine;

		return *this;
	}
}
