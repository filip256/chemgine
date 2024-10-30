#include "FileParser.hpp"
#include "Parsers.hpp"
#include "StringUtils.hpp"
#include "PathUtils.hpp"
#include "OOLDefRepository.hpp"
#include "DefinitionParser.hpp"
#include "FileStore.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

using namespace Def;

FileParser::FileParser(
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
		Log(this).error("Failed to open file for read: '{0}'.", currentFile);
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

FileParser::~FileParser() noexcept
{
	if (stream.is_open())
	{
		Log(this).warn("Incomplete parsing on file: '{0}'.", currentFile);
		stream.close();
	}
}

void FileParser::include(const std::string& filePath)
{
	const auto status = fileStore.getFileStatus(filePath);
	if (status == ParseStatus::COMPLETED) // skip already parsed file
		return;

	if (status == ParseStatus::STARTED)
	{
		Log(this).error("Encountered cyclic dependency on file: '{0}', at: {1}:{2}.", filePath, currentFile, currentLine);
		return;
	}

	subParser = std::make_unique<FileParser>(filePath, fileStore, oolDefinitions);
}

void FileParser::closeSubparser()
{
	// inherit include aliases
	includeAliases.merge(subParser->includeAliases);
	for (const auto& a : subParser->includeAliases)
		if(includeAliases[a.first] != a.second)
			Log(this).warn("Overwritten already defined include alias: '{0}: {1}' from included file, at: {2}:{3}.", a.first, a.second, currentFile, currentLine);
	
	subParser->stream.close();
	subParser.reset(nullptr);
}

bool FileParser::isOpen() const
{
	return stream.is_open();
}

Def::Location FileParser::getCurrentLocalLocation() const
{
	return isOpen() ? Def::Location(currentFile, currentLine) :
		Def::Location::createEOF(currentFile);
}

Def::Location FileParser::getCurrentGlobalLocation() const
{
	return subParser ? subParser->getCurrentGlobalLocation() :
		getCurrentLocalLocation();
}

void FileParser::forceFinish()
{
	if (isOpen())
	{
		fileStore.setFileStatus(currentFile, ParseStatus::COMPLETED);
		stream.close();
	}
}

std::string FileParser::nextLocalLine()
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

std::string FileParser::nextGlobalLine()
{
	// finish includes first
	if (subParser)
	{
		const auto subLine = subParser->nextGlobalLine();
		if (subParser->isOpen())
			return subLine;

		closeSubparser();
	}

	// main file
	while(true)
	{
		const auto line = nextLocalLine();
		if (line.empty())
			break;

		// include
		if (line.starts_with(Def::Syntax::Include))
		{
			const auto pathEnd = line.find(Def::Syntax::IncludeAs, Def::Syntax::Include.size());
			auto path = Utils::normalizePath(
				line.substr(Def::Syntax::Include.size(), pathEnd - Def::Syntax::Include.size()));

			// append dir
			if (path.starts_with("~/"))
				path = Utils::combinePaths(Utils::extractDirName(currentFile), path.substr(1));

			include(path);
			return nextGlobalLine();
		}

		return line;
	}

	forceFinish();
	return "";
}

std::pair<std::string, Def::Location> FileParser::nextDefinitionLine()
{
	// finish includes first
	if (subParser)
	{
		auto subLine = subParser->nextDefinitionLine();
		if (subLine.first.size())
			return subLine;

		closeSubparser();
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
		if (line.starts_with(Def::Syntax::Include))
		{
			const auto pathEnd = line.find(Def::Syntax::IncludeAs, Def::Syntax::Include.size());
			auto path = Utils::normalizePath(
				line.substr(Def::Syntax::Include.size(), pathEnd - Def::Syntax::Include.size()));

			// append dir
			if (path.starts_with("~/"))
				path = Utils::combinePaths(Utils::extractDirName(currentFile), path.substr(1));

			if (path.empty())
			{
				Log(this).error("Missing include path after '{0}' keyword, at: {1}:{2}.", Def::Syntax::Include, currentFile, currentLine);
				continue;
			}

			if (pathEnd != std::string::npos)
			{
				auto alias = Utils::strip(line.substr(pathEnd + Def::Syntax::IncludeAs.size()));
				if (alias.empty())
				{
					Log(this).error("Missing include alias after '{0}' keyword, at: {1}:{2}.", Def::Syntax::IncludeAs, currentFile, currentLine);
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

				// skip comments
				if (newLine.starts_with("::"))
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
	return std::make_pair("", Def::Location::createEOF(currentFile));
}

std::optional<Def::Object> FileParser::nextDefinition()
{
	// finish includes first
	if (subParser)
	{
		auto subDef = subParser->nextDefinition();
		if (subDef)
			return subDef;

		// continue parsing until EoF, even if error occurs
		if (subParser->isOpen() == false)
			closeSubparser();
	}

	auto [line, location] = nextDefinitionLine();
	if (location.isEOF())
		return std::nullopt;

	// TODO: remove dirty trick to pass subparser's inlcude aliases to this's parser (needed for the first def in a file)
	return Def::parse<Def::Object>(
		line, std::move(location), subParser ? subParser->includeAliases : includeAliases, oolDefinitions);
}
