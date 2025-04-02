#include "Log.hpp"

#include "Casts.hpp"
#include "STLUtils.hpp"
#include "PathUtils.hpp"
#include "BuildUtils.hpp"

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
	std::string&& address,
	std::string&& sourceName
) noexcept :
	address(std::move(address)),
	sourceName(std::move(sourceName))
{}

void LogBase::addContextIndent()
{
	OS::setTextColor(OS::Color::DarkGrey);
	for (uint8_t i = 0; i < contexts; ++i)
		outputStream << contextIndent;
	OS::setTextColor(OS::Color::White);
}

std::string LogBase::getSourceIdentifier(const std::source_location& location, const LogType type) const
{
	std::string sourceStr;

	if (printNameLevel.has(type) && sourceName.size())
	{
		sourceStr += sourceName;
		if (printAddressLevel.has(type) && address.size())
			sourceStr += "<" + address + '>';
	}

	if (printLocationLevel.has(type))
	{
		if (sourceStr.size())
			sourceStr += '|';

		// Trim the full path to start from the root of the project.
		constexpr std::string_view basePath = "Chemgine";
		const char* pathBegin = std::strstr(location.file_name(), basePath.data());
		if (not pathBegin)
			pathBegin = location.file_name();

		std::string pathStr(pathBegin);
		Utils::normalizePath(pathStr);

		sourceStr += pathStr + ':' + std::to_string(location.line());
	}

	return sourceStr;
}

void LogBase::logFormatted(const std::string& msg, const std::source_location& location, const LogType type) const
{
	// Exit folded log sequence.
	if (foldCount != static_cast<size_t>(-1) && not msg.starts_with('\r'))
	{
		foldCount = static_cast<size_t>(-1);
		outputStream << '\n';
	}

	// Dummy log (may be used to turn off folded logging).
	if (msg == "\0")
		return;

	uint16_t suffixSize = 0;

	if (foldCount == static_cast<size_t>(-1))
	{
		addContextIndent();

		switch (type)
		{
		case LogType::FATAL:
			OS::setTextColor(OS::Color::DarkRed);
			outputStream << "FATAL: ";
			suffixSize += 7;
			break;
		case LogType::ERROR:
			OS::setTextColor(OS::Color::Red);
			outputStream << "ERROR: ";
			suffixSize += 7;
			break;
		case LogType::WARN:
			OS::setTextColor(OS::Color::DarkYellow);
			outputStream << "WARN: ";
			suffixSize += 6;
			break;
		case LogType::SUCCESS:
			OS::setTextColor(OS::Color::Green);
			outputStream << "SUCCESS: ";
			suffixSize += 9;
			break;
		case LogType::INFO:
			OS::setTextColor(OS::Color::Cyan);
			outputStream << "INFO: ";
			suffixSize += 6;
			break;
		case LogType::DEBUG:
			OS::setTextColor(OS::Color::Magenta);
			outputStream << "DEBUG: ";
			suffixSize += 7;
			break;
		case LogType::TRACE:
			OS::setTextColor(OS::Color::DarkBlue);
			outputStream << "TRACE: ";
			suffixSize += 7;
			break;
		default:
			Log<LogBase>().fatal("Undefined log type: {0} {1}", underlying_cast(type));
		}

		if (const auto sourceIdentifier = getSourceIdentifier(location, type); sourceIdentifier.size())
		{
			OS::setTextColor(OS::Color::DarkGrey);
			outputStream << '[' << sourceIdentifier << "] ";
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
				outputStream << '\b';
		}

		outputStream << msg.substr(1);
		foldCount = msg.size() - 1;
	}
	else
	{
		// Normal log
		const auto splitMsg = Utils::split(msg, '\n', false);
		outputStream << splitMsg.front() << '\n';

		const std::string suffixSpace(suffixSize, ' ');
		for (size_t i = 1; i < splitMsg.size(); ++i)
		{
			addContextIndent();
			outputStream << suffixSpace << splitMsg[i] << '\n';
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
	const auto level = std::min(logLevel, newLevel);

	hideStack.emplace_back(logLevel);
	logLevel = level;
}

void LogBase::unhide()
{
	if (hideStack.empty())
		return;

	logLevel = hideStack.back();
	hideStack.pop_back();
}

void LogBase::breakline()
{
	outputStream << "\n.........................................................................\n\n";
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

	return Utils::find(typeMap, str);
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
