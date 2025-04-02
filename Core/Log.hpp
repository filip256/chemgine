#pragma once

#include "TerminalUtils.hpp"
#include "FormatUtils.hpp"
#include "MetaUtils.hpp"
#include "TypeUtils.hpp"
#include "FlagField.hpp"

#include <regex>
#include <string>
#include <vector>
#include <format>
#include <cstdint>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <source_location>

#define CHG_LOG_ERROR
#define CHG_LOG_WARN
#define CHG_LOG_SUCCESS
#define CHG_LOG_INFO

#ifndef NDEBUG
	#define CHG_LOG_DEBUG
	#define CHG_LOG_TRACE
#endif

#define CHG_DELAYED_EVAL(func) [&]() { return func; }


enum class LogType : uint8_t
{
	NONE = 0,
	FATAL = 1 << 0,
	ERROR = 1 << 1,
	WARN = 1 << 2,
	SUCCESS = 1 << 3,
	INFO = 1 << 4,
	DEBUG = 1 << 5,
	TRACE = 1 << 6,
	ALL = 0xFF
};


class LogFormat
{
private:
	const char* format;
	std::source_location location;

public:
	LogFormat(
		const char* format,
		std::source_location&& location = std::source_location::current()
	) noexcept;
	LogFormat(
		const std::string& format,
		std::source_location&& location = std::source_location::current()
	) noexcept;
	LogFormat(const LogFormat&) = delete;
	LogFormat(LogFormat&&) = default;

	const char* getFormat() const;
	const std::source_location& getLocation() const;
};


class LogBase
{
private:
	const std::string address;
	const std::string sourceName;

	static uint8_t contexts;
	static size_t foldCount;
	static std::vector<LogType> hideStack;

	static void addContextIndent();

	std::string getSourceIdentifier(const std::source_location& location, const LogType type) const;

	template <class... Args>
	void log(const LogFormat format, const LogType type, Args&&... args) const;

protected:
	void logFormatted(const std::string& msg, const std::source_location& location, const LogType type) const;

	LogBase(
		std::string&& address,
		std::string&& sourceName
	) noexcept;
	LogBase(const LogBase&) = delete;
	LogBase(LogBase&&) = delete;

public:
	static LogType logLevel;
	static FlagField<LogType> printNameLevel;
	static FlagField<LogType> printAddressLevel;
	static FlagField<LogType> printLocationLevel;
	static std::regex logSourceFilter;
	static std::ostream& outputStream;
	static std::string contextIndent;

	template <class... Args>
	[[noreturn]] void fatal(LogFormat format, Args&&... args) const;
	template <class... Args>
	void error(LogFormat format, Args&&... args) const;
	template <class... Args>
	void warn(LogFormat format, Args&&... args) const;
	template <class... Args>
	void success(LogFormat format, Args&&... args) const;
	template <class... Args>
	void info(LogFormat format, Args&&... args) const;
	template <class... Args>
	void debug(LogFormat format, Args&&... args) const;
	template <class... Args>
	void trace(LogFormat format, Args&&... args) const;

	static void nest();
	static void unnest();

	static void hide(const LogType newLevel = LogType::NONE);
	static void unhide();

	static void breakline();

	static std::optional<LogType> parseLogType(const std::string& str);
	static bool isLogTypeEnabled(const LogType type);
};

template <class... Args>
void LogBase::log(const LogFormat format, const LogType type, Args&&... args) const
{
	if (type > logLevel || (type != LogType::FATAL && not std::regex_match(sourceName, logSourceFilter)))
		return;

	// Values retuned from delayed-call args are temporary and need storage, normal args are stored as references.
	const auto argStorage = std::make_tuple(Utils::invokeOrForward(std::forward<Args>(args))...);
	const auto formatStr = format.getFormat();

	const auto message = std::apply(
		[&](const auto&... pArgs) {
			return std::vformat(formatStr, std::make_format_args(pArgs...));
		},
		argStorage
	);

	logFormatted(message, format.getLocation(), type);
}

template <class... Args>
void LogBase::fatal(LogFormat format, Args&&... args) const
{
	log(std::move(format), LogType::FATAL, std::forward<Args>(args)...);

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
void LogBase::error(LogFormat format, Args&&... args) const
{
#ifdef CHG_LOG_ERROR
	log(std::move(format), LogType::ERROR, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::warn(LogFormat format, Args&&... args) const
{
#ifdef CHG_LOG_WARN
	log(std::move(format), LogType::WARN, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::success(LogFormat format, Args&&... args) const
{
#ifdef CHG_LOG_SUCCESS
	log(std::move(format), LogType::SUCCESS, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::info(LogFormat format, Args&&... args) const
{
#ifdef CHG_LOG_INFO
	log(std::move(format), LogType::INFO, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::debug(LogFormat format, Args&&... args) const
{
#ifdef CHG_LOG_DEBUG
	log(std::move(format), LogType::DEBUG, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::trace(LogFormat format, Args&&... args) const
{
#ifdef CHG_LOG_TRACE
	log(std::move(format), LogType::TRACE, std::forward<Args>(args)...);
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
	LogBase(address ? Utils::toHex(address) : "", Utils::getTypeName<SourceT>())
{}
