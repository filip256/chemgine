#include "io/Log.hpp"

#include "utils/Casts.hpp"
#include "utils/STL.hpp"
#include "utils/Path.hpp"
#include "utils/Build.hpp"
#include "utils/String.hpp"
#include "utils/Concurrency.hpp"
#include "io/ColoredString.hpp"
#include "global/Charset.hpp"

#include <unordered_map>

LogFormat::LogFormat(
	const char* format,
	std::source_location&& location
) noexcept :
	format(format),
	location(std::move(location))
{}

LogFormat::LogFormat(
	const std::string& format,
	std::source_location&& location
) noexcept :
	LogFormat(format.c_str(), std::move(location))
{}

const char* LogFormat::getFormat() const
{
	return format;
}

const std::source_location& LogFormat::getLocation() const
{
	return location;
}

uint8_t LogBase::contexts = 0;
size_t LogBase::foldCount = static_cast<size_t>(-1);
std::vector<LogType> LogBase::hideStack = {};

LogBase::LogBase(
	const void* address,
	const std::type_index sourceType
) noexcept :
	address(address),
	sourceType(sourceType)
{}

void LogBase::addContextIndent()
{
	OS::setTextColor(OS::Color::DarkGrey);
	for (uint8_t i = 0; i < contexts; ++i)
		settings().outputStream << ASCII::MiddleDot1 <<"   ";
	OS::setTextColor(OS::Color::White);
}

std::string LogBase::getSourceIdentifier(const std::source_location& location, const LogType type) const
{
	std::string sourceStr;

	if (settings().printNameLevel.has(type) && sourceType != typeid(void))
	{
		sourceStr += utils::demangleTypeName(sourceType.name());

		if (settings().printAddressLevel.has(type) && address)
			sourceStr += '<' + utils::toHex(address) + '>';
	}

	if (settings().printLocationLevel.has(type))
	{
		if (sourceStr.size())
			sourceStr += '|';

		auto pathStr = utils::getRelativePathToProjectRoot(location.file_name());
		utils::normalizePath(pathStr);

		sourceStr += pathStr + ':' + std::to_string(location.line());
	}

	return sourceStr;
}

void LogBase::logFormatted(const std::string& msg, const std::source_location& location, const LogType type) const
{
	CHG_MUTEX_LOCK();

	auto& out = settings().outputStream;

	// Exit folded log sequence.
	if (foldCount != static_cast<size_t>(-1) && not msg.starts_with('\r'))
	{
		foldCount = static_cast<size_t>(-1);
		out << '\n';
	}

	// Dummy log (may be used to turn off folded logging).
	if (msg == "\0")
		return;

	uint16_t suffixSize = 0;

	if (foldCount == static_cast<size_t>(-1))
	{
		addContextIndent();

		static const std::unordered_map<LogType, ColoredString> tags
		{
			{LogType::FATAL, ColoredString("FATAL:", OS::Color::DarkRed)},
			{LogType::ERROR, ColoredString("ERROR:", OS::Color::Red)},
			{LogType::WARN, ColoredString("WARN:", OS::Color::DarkYellow)},
			{LogType::SUCCESS, ColoredString("SUCCESS:", OS::Color::Green)},
			{LogType::INFO, ColoredString("INFO:", OS::Color::Cyan)},
			{LogType::DEBUG, ColoredString("DEBUG:", OS::Color::Magenta)},
			{LogType::TRACE, ColoredString("TARCE:", OS::Color::DarkBlue)},
		};

		if (const auto tagIt = tags.find(type); tagIt != tags.end())
		{
			out << tagIt->second << ' ';
			suffixSize += static_cast<uint8_t>(tagIt->second.size() + 1);
		}
		else
			Log<LogBase>().fatal("Unknown log type: {0}.", underlying_cast(type));

		if (const auto sourceIdentifier = getSourceIdentifier(location, type); sourceIdentifier.size())
		{
			OS::setTextColor(OS::Color::DarkGrey);
			out << '[' << sourceIdentifier << "] ";
			suffixSize += static_cast<uint16_t>(sourceIdentifier.size() + 3);
		}

		OS::setTextColor(OS::Color::White);
	}

	if (msg.starts_with('\r'))
	{
		// Folded log
		// Don't backspace on the first folded log.
		if (foldCount != static_cast<size_t>(-1))
		{
			for (size_t i = 0; i < foldCount; ++i)
				out << '\b';
		}

		out << msg.c_str() + 1;
		foldCount = msg.size() - 1;
	}
	else
	{
		// Normal log
		const auto splitMsg = utils::split(msg, '\n', false);
		out << splitMsg.front() << '\n';

		const std::string suffixSpace(suffixSize, ' ');
		for (size_t i = 1; i < splitMsg.size(); ++i)
		{
			addContextIndent();
			out << suffixSpace << splitMsg[i] << '\n';
		}
	}
}

void LogBase::nest()
{
	if (contexts < 255)
		++contexts;
}

void LogBase::unnest()
{
	if(contexts > 0)
		--contexts;
}

void LogBase::hide(const LogType newLevel)
{
	const auto level = std::min(settings().logLevel, newLevel);

	hideStack.emplace_back(settings().logLevel);
	settings().logLevel = level;
}

void LogBase::unhide()
{
	if (hideStack.empty())
		return;

	settings().logLevel = hideStack.back();
	hideStack.pop_back();
}

void LogBase::breakline()
{
	settings().outputStream << "\n.........................................................................\n\n";
}

std::optional<LogType> LogBase::parseLogType(const std::string& str)
{
	static const std::unordered_map<std::string, LogType> typeMap
	{
		{"NONE", LogType::NONE},
		{"FATAL", LogType::FATAL},
		{"ERROR", LogType::ERROR},
		{"WARN", LogType::WARN},
		{"SUCCESS", LogType::SUCCESS},
		{"INFO", LogType::INFO},
		{"DEBUG", LogType::DEBUG},
		{"TRACE", LogType::TRACE},
		{"ALL", LogType::ALL},
	};

	return utils::find(typeMap, str);
}

bool LogBase::isLogTypeEnabled(const LogType type)
{
	switch (type)
	{
	case LogType::FATAL:
		return true;
	case LogType::ERROR:
#ifdef CHG_LOG_ERROR
		return true;
#else
		return false;
#endif
	case LogType::WARN:
#ifdef CHG_LOG_WARN
		return true;
#else
		return false;
#endif
	case LogType::SUCCESS:
#ifdef CHG_LOG_SUCCESS
		return true;
#else
		return false;
#endif
	case LogType::INFO:
#ifdef CHG_LOG_INFO
		return true;
#else
		return false;
#endif
	case LogType::DEBUG:
#ifdef CHG_LOG_DEBUG
		return true;
#else
		return false;
#endif
	case LogType::TRACE:
#ifdef CHG_LOG_TRACE
		return true;
#else
		return false;
#endif
	default:
		Log<LogBase>().fatal("Undefined log type: {0}", underlying_cast(type));
		CHG_UNREACHABLE();
	}
}

LogBase::Settings::Settings(
	LogType logLevel,
	FlagField<LogType> printNameLevel,
	FlagField<LogType> printAddressLevel,
	FlagField<LogType> printLocationLevel,
	std::regex&& logSourceFilter,
	std::ostream& outputStream
) noexcept :
	logLevel(logLevel),
	printNameLevel(printNameLevel),
	printAddressLevel(printAddressLevel),
	printLocationLevel(printLocationLevel),
	logSourceFilter(std::move(logSourceFilter)),
	outputStream(outputStream)
{
#ifdef CHG_UNSINK_STDIO
	std::ios::sync_with_stdio(false);
#endif
}

LogBase::Settings& LogBase::settings()
{
	static Settings settings
	(
		LogType::ALL, // logLevel
		FlagField(LogType::DEBUG) | LogType::WARN | LogType::ERROR | LogType::FATAL, // printNameLevel
		FlagField(LogType::DEBUG) | LogType::WARN | LogType::ERROR | LogType::FATAL, // printAddressLevel
		FlagField(LogType::DEBUG) | LogType::WARN | LogType::ERROR | LogType::FATAL, // printLocationLevel
		std::regex(), // logSourceFilter
		std::clog // outputStream
	);

	return settings;
}
