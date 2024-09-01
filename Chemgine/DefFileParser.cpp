#include "DefFileParser.hpp"
#include "StringUtils.hpp"
#include "PathUtils.hpp"
#include "FileStore.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

using namespace Keywords;

DefFileParser::DefFileParser(
	const std::string& filePath,
	FileStore& fileStore
) noexcept :
	currentFile(Utils::normalizePath(filePath)),
	stream(currentFile),
	fileStore(fileStore)
{
	if (stream.is_open() == false)
	{
		Log(this).error("Failed to open file: '{0}'.", currentFile);
		return;
	}

	if(fileStore.getFileStatus(filePath) == ParseStatus::PARSED)
	{
		Log(this).warn("Skipping already loaded file: '{0}'.", filePath);
		forceFinish();
		return;
	}

	fileStore.setFileStatus(currentFile, ParseStatus::STARTED);
}

DefFileParser::~DefFileParser() noexcept
{
	if (stream.is_open())
	{
		Log(this).warn("Incomplete parsing on file: '{0}'.", currentFile);
		stream.close();
	}
}

void DefFileParser::include(const std::string& filePath)
{
	const auto status = fileStore.getFileStatus(filePath);
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

	subParser = std::make_unique<DefFileParser>(filePath, fileStore);
}

bool DefFileParser::isOpen() const
{
	return stream.is_open();
}

DefinitionLocation DefFileParser::getCurrentLocalLocation() const
{
	return isOpen() ? DefinitionLocation(currentFile, currentLine) :
		DefinitionLocation::createEOF(currentFile);
}

DefinitionLocation DefFileParser::getCurrentGlobalLocation() const
{
	return subParser ? subParser->getCurrentGlobalLocation() :
		getCurrentLocalLocation();
}

void DefFileParser::forceFinish()
{
	if (isOpen())
	{
		fileStore.setFileStatus(currentFile, ParseStatus::PARSED);
		stream.close();
	}
}

std::string DefFileParser::nextLocalLine()
{
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

std::string DefFileParser::nextGlobalLine()
{
	// finish includes first
	if (subParser)
	{
		const auto subLine = subParser->nextGlobalLine();
		if (subParser->isOpen())
			return subLine;

		subParser.reset(nullptr);
	}

	// main file
	while(true)
	{
		const auto line = nextLocalLine();
		if (line.empty())
			break;

		// include
		if (line.starts_with(Syntax::Include))
		{
			const auto pathEnd = line.find(Syntax::IncludeAs, Syntax::Include.size());
			auto path = Utils::normalizePath(
				line.substr(Syntax::Include.size(), pathEnd - Syntax::Include.size()));

			// append dir
			if (path.starts_with(":/"))
				path = Utils::combinePaths(Utils::extractDirName(currentFile), path.substr(1));

			include(path);
			return nextGlobalLine();
		}

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

		includeAliases.merge(subParser->includeAliases); // inherit include aliases
		for (const auto& a : subParser->includeAliases)
			Log(this).warn("Ignored already defined include alias: '{0}: {1}' from included file, at: {2}:{3}.", a.first, a.second, currentFile, currentLine);
		subParser.reset(nullptr);
	}

	// main file
	while(true)
	{
		auto line = nextLocalLine();
		if (line.empty())
			break;

		// single-line comment
		if (line.starts_with("::"))
			continue;

		// multi-line comment
		if (line.starts_with(":."))
		{
			bool commentClosed = false;
			while(true)
			{
				line = nextLocalLine();
				if (line.empty())
					break;

				if (line.ends_with(".:"))
				{
					commentClosed = true;
					break;
				}
			};


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

		// include
		if (line.starts_with(Syntax::Include))
		{
			const auto pathEnd = line.find(Syntax::IncludeAs, Syntax::Include.size());
			auto path = Utils::normalizePath(
				line.substr(Syntax::Include.size(), pathEnd - Syntax::Include.size()));

			// append dir
			if (path.starts_with(":/"))
				path = Utils::combinePaths(Utils::extractDirName(currentFile), path.substr(1));

			if (path.empty())
			{
				Log(this).error("Missing include path after '{0}' keyword, at: {1}:{2}.", Syntax::Include, currentFile, currentLine);
				continue;
			}

			if (pathEnd != std::string::npos)
			{
				auto alias = Utils::strip(line.substr(pathEnd + Syntax::IncludeAs.size()));
				if (alias.empty())
				{
					Log(this).error("Missing include alias after '{0}' keyword, at: {1}:{2}.", Syntax::IncludeAs, currentFile, currentLine);
					continue;
				}

				if (auto status = includeAliases.emplace(std::move(alias), path); status.second == false)
				{
					Log(this).warn("Redefinition of an existing include alias: '{0}: {1}', at: {2}:{3}.", alias, status.first->second, currentFile, currentLine);
					status.first->second = path;
				}
			}

			include(path);
			return nextDefinitionLine();
		}

		// defs
		if (line.starts_with('_'))
		{
			auto location = getCurrentLocalLocation();

			// single-line def
			if (line.ends_with(';'))
				return std::make_pair(line, std::move(location));

			// multi-line def
			while(true)
			{
				const auto newLine = nextLocalLine();
				if (newLine.empty())
					break;

				if (newLine.starts_with('_')) // reached next def, without terminator
					continue;

				line += ' ' + newLine;
				if (newLine.ends_with(';'))
					return std::make_pair(line, std::move(location));
			};

			Log(this).error("Missing definition terminator: ';', at: {0}.", location.toString());
			continue;
		}

		Log(this).error("Unknown synthax: '{0}', at: {1}:{2}.", line, currentFile, currentLine);
	};

	forceFinish();
	return std::make_pair("", DefinitionLocation::createEOF(currentFile));
}

std::optional<DefinitionObject> DefFileParser::nextDefinition()
{
	static const std::unordered_map<std::string, DefinitionType> typeMap
	{
		{Types::Spline, DefinitionType::SPLINE},
		{Types::Atom, DefinitionType::ATOM},
		{Types::Radical, DefinitionType::RADICAL},
		{Types::Molecule, DefinitionType::MOLECULE},
		{Types::Reaction, DefinitionType::REACTION},
		{Types::Labware, DefinitionType::LABWARE},
	};

	// finish includes first
	if (subParser)
	{
		auto subDef = subParser->nextDefinition();
		if (subDef.has_value())
			return subDef;

		// continue parsing until EoF, even if error occurs
		if(subParser->isOpen() == false)
			subParser.reset(nullptr);
	}

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
			Log(this).error("Missing identifier terminator: '>', at: {0}.", location.toString());
			return std::nullopt;
		}

		if (idEnd - typeEnd < 2)
		{
			Log(this).error("Definition with empty identifier, at: {0}.", location.toString());
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

	const auto specifierBegin = line.find(':', idEnd);
	const auto specifierEnd = line.find('{', specifierBegin + 1);
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
	const auto idStr = idEnd != typeEnd ? Utils::strip(line.substr(typeEnd + 1, idEnd - typeEnd - 1)) : "";
	if (const auto illegalIdx = idStr.find_first_of(" .~;:'\"<>(){}~`!@#$%^&*()-+[]{}|?,/\\"); illegalIdx != std::string::npos)
	{
		Log(this).error("Identifier: '{0}' contains illegal symbol: '{1}', at: {2}.", idStr, idStr[illegalIdx], location.toString());
		return std::nullopt;
	}

	// parse specifier
	auto specifierStr = Utils::strip(line.substr(specifierBegin + 1, specifierEnd - specifierBegin - 1));
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

		if (value.starts_with('$'))
		{
			if (const auto aliasEnd = value.find('@', 1); aliasEnd != std::string::npos)
			{
				// expand include aliases
				const auto alias = value.substr(1, aliasEnd - 1);
				if (alias.empty())
				{
					Log(this).error("Empty include alias on value: '{0}', at: {1}.", value, location.toString());
					return std::nullopt;
				}

				const auto aliasIt = includeAliases.find(alias);
				if (aliasIt == includeAliases.end())
				{
					Log(this).error("Undefined include alias: '{0}', at: {1}.", alias, location.toString());
					return std::nullopt;
				}

				value.replace(1, alias.size(), aliasIt->second);
			}
		}

		properties.emplace(std::make_pair(std::move(name), std::move(value)));
	}

	return std::optional<DefinitionObject>(std::in_place,
		type,
		idStr.size() ? location.getFile() + '@' + idStr : "",
		std::move(specifierStr),
		std::move(properties),
		std::move(location)
	);
}

