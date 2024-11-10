#pragma once

#include "LogType.hpp"
#include "Linguistics.hpp"
#include "StringUtils.hpp"
#include "TerminalUtils.hpp"

#include <regex>
#include <string>
#include <vector>
#include <format>
#include <typeinfo>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#ifdef NDEBUG
	#define CHEM_LOG_ERROR
	#define CHEM_LOG_WARN
	#define CHEM_LOG_INFO
#else
	#define CHEM_LOG_ERROR
	#define CHEM_LOG_WARN
	#define CHEM_LOG_SUCCESS
	#define CHEM_LOG_INFO
	#define CHEM_LOG_DEBUG
	#define CHEM_LOG_TRACE
#endif

class LogBase
{
private:
	const void* const location;

protected:
	static uint8_t contexts;
	static size_t foldCount;
	static std::vector<LogType> hideStack;

	LogBase(const void* location = nullptr) noexcept;

	std::string getAddress() const;

	static void addContextIndent();

public:
	static void nest();
	static void unnest();

	static void hide(const LogType newLevel = LogType::NONE);
	static void unhide();

	static void breakline();

	static LogType logLevel;
	static LogType printNameLevel;
	static LogType printAddressLevel;

	static std::regex logSourceFilter;

	static std::ostream& outputStream;

	static std::string contexIndent;

	LogBase() = delete;
};


template<typename SourceT = void>
class Log : public LogBase
{
private:
	static std::string getTypeName();

	const Log& log(const std::string& msg, const LogType type) const;
	__declspec(noreturn) void fatalExit(const std::string& msg) const;

public:
	Log(const SourceT* location = nullptr) noexcept;

	template <class... Args>
	__declspec(noreturn) void fatal(std::format_string<Args...> format, Args&&... args) const;
	template <class... Args>
	const Log& error(std::format_string<Args...> format, Args&&... args) const;
	template <class... Args>
	const Log& warn(std::format_string<Args...> format, Args&&... args) const;
	template <class... Args>
	const Log& success(std::format_string<Args...> format, Args&&... args) const;
	template <class... Args>
	const Log& info(std::format_string<Args...> format, Args&&... args) const;
	template <class... Args>
	const Log& debug(std::format_string<Args...> format, Args&&... args) const;
	template <class... Args>
	const Log& trace(std::format_string<Args...> format, Args&&... args) const;
};

template<typename SourceT>
Log<SourceT>::Log(const SourceT* location) noexcept :
	LogBase(location)
{}

template<typename SourceT>
std::string Log<SourceT>::getTypeName()
{
	if constexpr (std::is_same<SourceT, void>())
		return "";

	auto name = std::string(typeid(SourceT).name());

	if (name.starts_with("class"))
		name = name.substr(5);

	Utils::strip(name);

	return name.size() ? name : "";
}

template<typename SourceT>
const Log<SourceT>& Log<SourceT>::log(const std::string& msg, const LogType type) const
{
	// exit folded log sequence
	if (foldCount != static_cast<size_t>(-1) && not msg.starts_with('\r'))
	{
		foldCount = static_cast<size_t>(-1);
		outputStream << '\n';
	}
	
	// dummy log (may be used to turn off folded logging)
	if (msg == "\0")
		return *this;

	const auto typeName = getTypeName();
	if (not std::regex_match(typeName, logSourceFilter))
		return *this;

	uint16_t suffixSize = 0;


	if (foldCount == static_cast<size_t>(-1))
	{
		addContextIndent();

		switch (type)
		{
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
			if (typeName.size())
			{
				OS::setTextColor(OS::Color::DarkGrey);
				outputStream << '[' << typeName;
				if (type <= printAddressLevel)
				{
					if (const auto addr = getAddress(); addr.size())
					{
						outputStream << " <" << addr << '>';
						suffixSize += static_cast<uint16_t>(addr.size() + 3);
					}
				}
				outputStream << "] ";
				suffixSize += static_cast<uint16_t>(typeName.size() + 3);
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

	return *this;
}

template<typename SourceT>
void Log<SourceT>::fatalExit(const std::string& msg) const
{
	// exit folded log sequence
	if (foldCount != static_cast<size_t>(-1) && msg.starts_with('\r') == false)
	{
		foldCount = static_cast<size_t>(-1);
		outputStream << '\n';
	}

	outputStream << '\n';
	OS::setTextColor(OS::Color::DarkRed);
	outputStream << "FATAL:   ";
	OS::setTextColor(OS::Color::White);

	const auto name = getTypeName();
	if (name.size())
	{
		outputStream << '[' << getTypeName();
		const auto addr = getAddress();
		if (addr.size())
			outputStream << ": " << getAddress();
		outputStream << "] ";
	}

	outputStream << msg << '\n';

	OS::setTextColor(OS::Color::DarkRed);
	outputStream << "\n   The execution was halted due to a fatal error!\n   Press ENTER to exit.\n";
	OS::setTextColor(OS::Color::White);

	throw std::runtime_error("Fatal error: '" + msg + "'.");

	const auto ignored = getchar();
	std::exit(EXIT_FAILURE);
}

template<typename SourceT>
template <class... Args>
void Log<SourceT>::fatal(std::format_string<Args...> format, Args&&... args) const
{
	return fatalExit(std::vformat(format.get(), std::make_format_args(args...)));
}

template<typename SourceT>
template <class... Args>
const Log<SourceT>& Log<SourceT>::error(std::format_string<Args...> format, Args&&... args) const
{
#ifdef CHEM_LOG_ERROR
	if (LogType::ERROR > logLevel)
		return *this;

	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::ERROR);
#else
	return *this;
#endif
}

template<typename SourceT>
template <class... Args>
const Log<SourceT>& Log<SourceT>::warn(std::format_string<Args...> format, Args&&... args) const
{
#ifdef CHEM_LOG_WARN
	if (LogType::WARN > logLevel)
		return *this;

	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::WARN);
#else
	return *this;
#endif
}

template<typename SourceT>
template <class... Args>
const Log<SourceT>& Log<SourceT>::success(std::format_string<Args...> format, Args&&... args) const
{
#ifdef CHEM_LOG_SUCCESS
	if (LogType::SUCCESS > logLevel)
		return *this;

	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::SUCCESS);
#else
	return *this;
#endif
}

template<typename SourceT>
template <class... Args>
const Log<SourceT>& Log<SourceT>::info(std::format_string<Args...> format, Args&&... args) const
{
#ifdef CHEM_LOG_INFO
	if (LogType::INFO > logLevel)
		return *this;

	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::INFO);
#else
	return *this;
#endif
}

template<typename SourceT>
template <class... Args>
const Log<SourceT>& Log<SourceT>::debug(std::format_string<Args...> format, Args&&... args) const
{
#ifdef CHEM_LOG_DEBUG
	if (LogType::DEBUG > logLevel)
		return *this;

	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::DEBUG);
#else
	return *this;
#endif
}

template<typename SourceT>
template <class... Args>
const Log<SourceT>& Log<SourceT>::trace(std::format_string<Args...> format, Args&&... args) const
{
#ifdef CHEM_LOG_TRACE
	if (LogType::TRACE > logLevel)
		return *this;

	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::TRACE);
#else
	return *this;
#endif
}
