#pragma once

#include <string>

namespace def
{
	class PrintSettings
	{
	public:
		const bool prettify;
		const std::string newLine;
		const std::string specifierSep;
		const std::string propertySep;
		const std::string propertyIndent;
		const std::string propertyBlockBegin;
		const std::string propertyBlockEnd;
		const std::string definitionEnd;

		PrintSettings(
			const bool prettify,
			std::string&& newLine,
			std::string&& specifierSep,
			std::string&& propertySep,
			std::string&& propertyIndent,
			std::string&& propertyBlockBegin,
			std::string&& propertyBlockEnd,
			std::string&& definitionEnd
		) noexcept;

		static const PrintSettings Minimal;
		static const PrintSettings Pretty;
	};
}
