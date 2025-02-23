#pragma once

#include "TerminalUtils.hpp"
#include "Linguistics.hpp"
#include "MetaUtils.hpp"
#include "TypeUtils.hpp"
#include "LogType.hpp"

#include <regex>
#include <string>
#include <vector>
#include <format>
#include <optional>
#include <iostream>
#include <stdexcept>

#ifdef NDEBUG
	#define CHG_LOG_ERROR
	#define CHG_LOG_WARN
	#define CHG_LOG_INFO
#else
	#define CHG_LOG_ERROR
	#define CHG_LOG_WARN
	#define CHG_LOG_SUCCESS
	#define CHG_LOG_INFO
	#define CHG_LOG_DEBUG
	#define CHG_LOG_TRACE
#endif

#define CHG_DELAYED_EVAL(func) [&]() { return func; }

class LogBase
{
protected:
	const std::string address;
	const std::string sourceName;

	static uint8_t contexts;
	static size_t foldCount;
	static std::vector<LogType> hideStack;

	LogBase(
		std::string&& address,
		std::string&& sourceName
	) noexcept;

	void log(const std::string& msg, const LogType type) const;
	template <class... Args>
	void log(const std::string& format, const LogType type, Args&&... args) const;

	static void addContextIndent();

public:
	static LogType logLevel;
	static LogType printNameLevel;
	static LogType printAddressLevel;
	static std::regex logSourceFilter;
	static std::ostream& outputStream;
	static std::string contexIndent;

	template <class... Args>
	__declspec(noreturn) void fatal(const std::string& format, Args&&... args) const;
	template <class... Args>
	void error(const std::string& format, Args&&... args) const;
	template <class... Args>
	void warn(const std::string& format, Args&&... args) const;
	template <class... Args>
	void success(const std::string& format, Args&&... args) const;
	template <class... Args>
	void info(const std::string& format, Args&&... args) const;
	template <class... Args>
	void debug(const std::string& format, Args&&... args) const;
	template <class... Args>
	void trace(const std::string& format, Args&&... args) const;

	static void nest();
	static void unnest();

	static void hide(const LogType newLevel = LogType::NONE);
	static void unhide();

	static void breakline();

	static std::optional<LogType> parseLogType(const std::string& str);
	static bool isLogTypeEnabled(const LogType type);
};

template <class... Args>
void LogBase::log(const std::string& format, const LogType type, Args&&... args) const
{
	if (type > logLevel || (type != LogType::FATAL && not std::regex_match(sourceName, logSourceFilter)))
		return;

	// Values retuned from delayed-call args are temporary and need storage, normal args are stored as references.
	const auto argStorage = std::make_tuple(Utils::invokeOrForward(std::forward<Args>(args))...);

	const auto message = std::apply(
		[&format](const auto&... pArgs) {
			return std::vformat(format, std::make_format_args(pArgs...));
		},
		argStorage
	);

	log(message, type);
}

template <class... Args>
void LogBase::fatal(const std::string& format, Args&&... args) const
{
	log(format, LogType::FATAL, std::forward<Args>(args)...);

	OS::setTextColor(OS::Color::DarkRed);
	outputStream << "\n   Execution aborted due to a fatal error.\n   Press ENTER to exit.\n";
	OS::setTextColor(OS::Color::White);

#ifndef NDEBUG
	throw std::runtime_error("Fatal error.");
#endif

	const auto ignored = getchar();
	std::exit(EXIT_FAILURE);
}

template <class... Args>
void LogBase::error(const std::string& format, Args&&... args) const
{
#ifdef CHG_LOG_ERROR
	log(format, LogType::ERROR, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::warn(const std::string& format, Args&&... args) const
{
#ifdef CHG_LOG_WARN
	log(format, LogType::WARN, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::success(const std::string& format, Args&&... args) const
{
#ifdef CHG_LOG_SUCCESS
	log(format, LogType::SUCCESS, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::info(const std::string& format, Args&&... args) const
{
#ifdef CHG_LOG_INFO
	log(format, LogType::INFO, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::debug(const std::string& format, Args&&... args) const
{
#ifdef CHG_LOG_DEBUG
	log(format, LogType::DEBUG, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::trace(const std::string& format, Args&&... args) const
{
#ifdef CHG_LOG_TRACE
	log(format, LogType::TRACE, std::forward<Args>(args)...);
#endif
}


template<typename SourceT = void>
class Log : public LogBase
{
public:
	Log(const SourceT* address = nullptr) noexcept;
};

template<typename SourceT>
Log<SourceT>::Log(const SourceT* address) noexcept :
	LogBase(address ? Linguistics::toHex(address) : "", Utils::getTypeName<SourceT>())
{}
