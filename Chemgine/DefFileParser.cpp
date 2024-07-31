#include "DefFileParser.hpp"
#include "StringUtils.hpp"
#include "PathUtils.hpp"
#include "DataStore.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

DefFileParser::DefFileParser(
	const std::string& filePath,
	DataStore& dataStore
) noexcept :
	currentFile(Utils::normalizePath(filePath)),
	stream(currentFile),
	dataStore(dataStore)
{
	if (stream.is_open())
	{
		dataStore.fileParseHistory.emplace(std::make_pair(currentFile, false));
		return;
	}

	Log(this).error("Failed to open file: '{0}'.", currentFile);
}

DefFileParser::~DefFileParser() noexcept
{
	if (stream.is_open())
	{
		Log(this).warn("Incomplete parsing on file: '{0}'.", currentFile);
		stream.close();
	}
}

bool DefFileParser::isOpen() const
{
	return stream.is_open();
}

void DefFileParser::include(const std::string& filePath)
{
	const auto status = dataStore.getFileStatus(filePath);
	if (status == ParseStatus::PARSED)
	{
		Log(this).warn("Skipping already included file: '{0}', at: {1}:{2}.", filePath, currentFile, currentLine);
		return;
	}

	if (status == ParseStatus::STARTED)
	{
		Log(this).error("Encountered cyclic dependency on file: '{0}', at: {1}:{2}.", filePath, currentFile, currentLine);
		return;
	}

	subParser.reset(new DefFileParser(filePath, dataStore));
}

void DefFileParser::forceFinish()
{
	if (stream.is_open())
	{
		dataStore.fileParseHistory.at(currentFile) = true;
		stream.close();
	}
}

std::string DefFileParser::nextLine()
{
	if (stream.is_open() == false)
		return "";

	std::string line;
	while (std::getline(stream, line))
	{
		++currentLine;

		Utils::strip(line);
		if (line.empty())
			continue;

		return line;
	}

	forceFinish();
	return "";
}

std::pair<std::string, DefinitionLocation> DefFileParser::nextDefinitionLine()
{
	// finish includes first
	if (subParser)
	{
		auto subLine = subParser->nextDefinitionLine();
		if (subLine.first.size())
			return subLine;

		subParser.reset(nullptr);
	}

	// main file
	do
	{
		auto line = nextLine();
		if (line.empty())
			break;

		// single-line comment
		if (line.starts_with("::"))
			continue;

		// multi-line comment
		if (line.starts_with(":."))
		{
			bool commentClosed = false;
			do
			{
				line = nextLine();
				if (line.empty())
					break;

				if (line.ends_with(".:"))
				{
					commentClosed = true;
					break;
				}
			} while (true);


			if (commentClosed)
				continue;

			Log(this).error("Missing multi-line definition terminator: '.:', at: {0}:{1}.", currentFile, currentLine);
		}

		// debug message
		if (line.starts_with(">>"))
		{
			line = line.substr(2);
			Utils::strip(line);
			Log(this).debug("{0}", line);
			continue;
		}

		// load
		if (line.starts_with(Keywords::Syntax::Include))
		{
			line = line.substr(7);
			Utils::normalizePath(line);

			if(line.starts_with(":/"))
				include(Utils::combinePaths(Utils::extractDirName(currentFile), line.substr(1)));
			else
				include(line);

			return nextDefinitionLine();
		}

		// defs
		if (line.starts_with('_'))
		{
			DefinitionLocation location(currentFile, currentLine);

			// single-line def
			if (line.ends_with(';'))
				return std::make_pair(line, std::move(location));

			// multi-line def
			do
			{
				const auto newLine = nextLine();
				if (newLine.empty())
					break;

				if (newLine.starts_with('_')) // reached next def, without terminator
					continue;

				line += ' ' + newLine;
				if (newLine.ends_with(';'))
					return std::make_pair(line, std::move(location));
			} while (true);

			Log(this).error("Missing multi-line definition terminator: ';', at: {0}.", location.toString());
			continue;
		}

		Log(this).error("Unknown synthax: '{0}', at: {1}:{2}.", line, currentFile, currentLine);

	} while (true);

	forceFinish();
	return std::make_pair("", DefinitionLocation::createEOF(currentFile));
}

std::optional<DefinitionObject> DefFileParser::nextDefinition()
{
	static const std::unordered_map<std::string, DefinitionType> typeMap
	{
		{Keywords::Types::Spline, DefinitionType::SPLINE},
		{Keywords::Types::Atom, DefinitionType::ATOM},
		{Keywords::Types::Radical, DefinitionType::RADICAL},
		{Keywords::Types::Molecule, DefinitionType::MOLECULE},
		{Keywords::Types::Reaction, DefinitionType::REACTION},
		{Keywords::Types::Labware, DefinitionType::LABWARE},
	};

	auto [line, location] = nextDefinitionLine();
	if (line.starts_with('_') == false)
		return std::nullopt;

	auto idEnd = std::string::npos;
	auto typeEnd = line.find('<', 1);
	if (typeEnd != std::string::npos)
	{
		idEnd = line.find('>', typeEnd + 1);
		if (idEnd == std::string::npos)
		{
			Log(this).error("Malformed definition: '{0}', at: {1}.", line, location.toString());
			return std::nullopt;
		}
	}
	else
		idEnd = typeEnd = line.find(':', 1);

	if (typeEnd == std::string::npos)
	{
		Log(this).error("Malformed definition: '{0}', at: {1}.", line, location.toString());
		return std::nullopt;
	}

	const auto specifierEnd = line.find('{', idEnd + 1);
	if (specifierEnd == std::string::npos)
	{
		Log(this).error("Malformed definition: '{0}', at: {1}.", line, location.toString());
		return std::nullopt;
	}

	const auto propertiesEnd = line.rfind('}', line.size() - 2);
	if (propertiesEnd == std::string::npos || propertiesEnd < specifierEnd)
	{
		Log(this).error("Malformed definition: '{0}', at: {1}.", line, location.toString());
		return std::nullopt;
	}

	// parse type
	const auto typeStr = Utils::strip(line.substr(1, typeEnd - 1));
	if (typeStr.empty())
	{
		Log(this).error("Definition with missing type: '{0}', at: {1}.", line, location.toString());
		return std::nullopt;
	}
	const auto typeIt = typeMap.find(typeStr);
	if(typeIt == typeMap.end())
	{
		Log(this).error("Definition with unknown type: '{0}', at: {1}.", typeStr, location.toString());
		return std::nullopt;
	}
	const auto type = typeIt->second;

	// parse identifier (optional)
	auto idStr = Utils::strip(line.substr(typeEnd + 1, idEnd - typeEnd - 1));

	// parse specifier
	auto specifierStr = Utils::strip(line.substr(idEnd + 1, specifierEnd - idEnd - 1));
	if (specifierStr.empty())
	{
		Log(this).error("Definition with missing specifier: '{0}', at: {1}.", line, location.toString());
		return std::nullopt;
	}

	// parse properties
	const auto propertiesStr = line.substr(specifierEnd + 1, propertiesEnd - specifierEnd - 1);
	if (propertiesStr.empty())
	{
		Log(this).error("Definition with missing properties block: '{0}', at: {1}.", line, location.toString());
		return std::nullopt;
	}

	const auto props = Utils::split(propertiesStr, ',', '{', '}', true);
	std::unordered_map<std::string, std::string> properties;
	properties.reserve(props.size());

	for (size_t i = 0; i < props.size(); ++i)
	{
		const auto pair = Utils::split(props[i], ':', '{', '}', true);
		if (pair.size() != 2)
		{
			Log(this).error("Definition with malformed property: '{0}', at: {1}.", props[i], location.toString());
			return std::nullopt;
		}

		auto name = Utils::strip(pair.front());
		if (name.empty())
		{
			Log(this).error("Definition property with missing name: '{0}', at: {1}.", props[i], location.toString());
			return std::nullopt;
		}

		auto value = Utils::strip(pair.back());
		if (value.empty())
		{
			Log(this).error("Definition property with missing value: '{0}', at: {1}.", props[i], location.toString());
			return std::nullopt;
		}

		properties.emplace(std::make_pair(std::move(name), std::move(value)));
	}

	return std::optional<DefinitionObject>(std::in_place,
		type,
		std::move(idStr),
		std::move(specifierStr),
		std::move(properties),
		std::move(location)
	);
}

