#pragma once

#include <regex>
#include <string>
#include <vector>
#include <format>
#include <typeinfo>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include "LogType.hpp"
#include "Linguistics.hpp"
#include "StringUtils.hpp"
#include "TerminalUtils.hpp"


#ifndef NDEBUG
	#define CHEM_LOG_ERROR
	#define CHEM_LOG_WARN
	#define CHEM_LOG_SUCCESS
	#define CHEM_LOG_INFO
	#define CHEM_LOG_DEBUG
	#define CHEM_LOG_TRACE
#else
	#define CHEM_LOG_ERROR
	#define CHEM_LOG_WARN
	#define CHEM_LOG_INFO
#endif


class LogBase
{
private:
	const void* const location;

protected:
	static uint8_t contexts;
	static size_t foldCount;
	static std::vector<std::string> cache;

	LogBase(const void* location = nullptr) noexcept;

	std::string getAddress() const;

public:
	static void nest();
	static void unnest();

	static void breakline();

	static void clearCache();

	static LogType logLevel;
	static LogType printNameLevel;
	static LogType printAddressLevel;

	static std::regex logSourceFilter;

	static std::ostream& outputStream;

	LogBase() = delete;
};


template<typename SourceT = void>
class Log : public LogBase
{
private:
	static std::string getTypeName();

	const Log& log(const std::string& msg, const LogType type) const;
	void fatalExit(const std::string& msg) const;

public:
	Log(const SourceT* location = nullptr) noexcept;

	template <class... Args>
	void fatal(std::format_string<Args...> format, Args&&... args) const;
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

	void cache(const std::string& str);
	void printCache();
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
	if (type > logLevel)
		return *this;

	const auto typeName = getTypeName();
	if (std::regex_match(typeName, logSourceFilter) == false)
		return *this;

	size_t suffixSize = 0;

	// exit folded log sequence
	if (foldCount != static_cast<size_t>(-1) && msg.starts_with('\r') == false)
	{
		foldCount = static_cast<size_t>(-1);
		outputStream << '\n';
	}

	if (foldCount == static_cast<size_t>(-1))
	{
		for (uint8_t i = 0; i < contexts; ++i)
			outputStream << "  ";
		suffixSize += contexts * 2;

		switch (type)
		{
		case LogType::ERROR:
			OS::setTextRed();
			outputStream << "ERROR:   ";
			break;
		case LogType::WARN:
			OS::setTextDarkYellow();
			outputStream << "WARN:    ";
			break;
		case LogType::SUCCESS:
			OS::setTextGreen();
			outputStream << "SUCCESS: ";
			break;
		case LogType::INFO:
			OS::setTextCyan();
			outputStream << "INFO:    ";
			break;
		case LogType::DEBUG:
			OS::setTextMagenta();
			outputStream << "DEBUG:   ";
			break;
		case LogType::TRACE:
			OS::setTextBlue();
			outputStream << "TRACE:   ";
			break;
		}
		suffixSize += 10;


		if (type != LogType::NONE)
			OS::setTextWhite();

		if (type <= printNameLevel)
		{
			if (typeName.size())
			{
				outputStream << '[' << typeName;
				if (type <= printAddressLevel)
				{
					if (const auto addr = getAddress(); addr.size())
					{
						outputStream << " <" << addr << '>';
						suffixSize += addr.size() + 3;
					}
				}
				outputStream << "]   ";
				suffixSize += typeName.size() + 5;
			}
		}
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
			outputStream << suffixSpace << splitMsg[i] << '\n';
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
	OS::setTextDarkRed();
	outputStream << "FATAL:   ";
	OS::setTextWhite();

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

	OS::setTextDarkRed();
	outputStream << "\n   The execution was halted due to a fatal error!\n   Press ENTER to exit.\n";
	OS::setTextWhite();

	throw std::runtime_error("Fatal error: '" + msg + "'.");

	getchar();
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
	return log(std::vformat(format.get(), std::make_format_args(args...)), LogType::TRACE);
#else
	return *this;
#endif
}

template<typename SourceT>
void Log<SourceT>::cache(const std::string& str)
{
#ifdef CHEM_LOG_TRACE
	LogBase::cache.emplace_back("");
	for (uint8_t i = 0; i < contexts; ++i)
		LogBase::cache.back() += "  ";
	LogBase::cache.back() += str;
#endif
}

template<typename SourceT>
void Log<SourceT>::printCache()
{
#ifdef CHEM_LOG_TRACE
	for (size_t i = 0; i < LogBase::cache.size(); ++i)
		log(LogBase::cache[i], LogType::TABLE);
#endif
}
