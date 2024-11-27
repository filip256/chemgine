#include "Log.hpp"
#include "LogType.hpp"

#include <unordered_map>

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

void LogBase::log(const std::string& msg, const LogType type) const
{
	// exit folded log sequence
	if (foldCount != static_cast<size_t>(-1) && not msg.starts_with('\r'))
	{
		foldCount = static_cast<size_t>(-1);
		outputStream << '\n';
	}

	// dummy log (may be used to turn off folded logging)
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
		}

		if (type <= printNameLevel)
		{
			if (sourceName.size())
			{
				OS::setTextColor(OS::Color::DarkGrey);
				outputStream << '[' << sourceName;
				if (type <= printAddressLevel)
				{
					if (address.size())
					{
						outputStream << " <" << address << '>';
						suffixSize += static_cast<uint16_t>(address.size() + 3);
					}
				}
				outputStream << "] ";
				suffixSize += static_cast<uint16_t>(sourceName.size() + 3);
			}
		}

		OS::setTextColor(OS::Color::White);
	}

	if (msg.starts_with('\r')) // folded log
	{
		// don't backspace on the first folded log
		if (foldCount != static_cast<size_t>(-1))
		{
			for (size_t i = 0; i < foldCount; ++i)
				outputStream << '\b';
		}

		outputStream << msg.substr(1);
		foldCount = msg.size() - 1;
	}
	else // normal log
	{
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

void LogBase::addContextIndent()
{
	OS::setTextColor(OS::Color::DarkGrey);
	for (uint8_t i = 0; i < contexts; ++i)
		outputStream << contexIndent;
	OS::setTextColor(OS::Color::White);
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

std::optional<LogType> LogBase::parseLogLevel(const std::string& str)
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

	if (const auto typeIt = typeMap.find(str); typeIt != typeMap.end())
		return typeIt->second;
	return std::nullopt;
}
