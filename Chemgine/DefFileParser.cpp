#include "DefFileParser.hpp"
#include "Parsers.hpp"
#include "StringUtils.hpp"
#include "PathUtils.hpp"
#include "OOLDefRepository.hpp"
#include "FileStore.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

using namespace Keywords;

DefFileParser::DefFileParser(
	const std::string& filePath,
	FileStore& fileStore,
	const OOLDefRepository& oolDefinitions
) noexcept :
	currentFile(Utils::normalizePath(filePath)),
	stream(currentFile),
	oolDefinitions(oolDefinitions),
	fileStore(fileStore)
{
	if (stream.is_open() == false)
	{
		Log(this).error("Failed to open file: '{0}'.", currentFile);
		return;
	}

	if(fileStore.getFileStatus(filePath) == ParseStatus::COMPLETED)
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
	if (status == ParseStatus::COMPLETED)
	{
		Log(this).warn("Skipping already included file: '{0}', at: {1}:{2}.", filePath, currentFile, currentLine);
		return;
	}

	if (status == ParseStatus::STARTED)
	{
		Log(this).error("Encountered cyclic dependency on file: '{0}', at: {1}:{2}.", filePath, currentFile, currentLine);
		return;
	}

	subParser = std::make_unique<DefFileParser>(filePath, fileStore, oolDefinitions);
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
		fileStore.setFileStatus(currentFile, ParseStatus::COMPLETED);
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
	if (location.isEOF())
		return std::nullopt;
	
	return Def::parse<DefinitionObject>(line, std::move(location), includeAliases, oolDefinitions);
}
