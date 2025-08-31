#pragma once

#include "structs/FlagField.hpp"
#include "utils/Exception.hpp"
#include "utils/Format.hpp"
#include "utils/Meta.hpp"
#include "utils/Terminal.hpp"
#include "utils/Type.hpp"

#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <regex>
#include <source_location>
#include <string>
#include <typeindex>
#include <vector>

//
// Log Level
//

#if defined(CHG_LOG_DEFAULT)
    #define CHG_LOG_ERROR
    #define CHG_LOG_WARN
    #define CHG_LOG_SUCCESS
    #define CHG_LOG_INFO

    #if not defined(NDEBUG)
        #define CHG_LOG_DEBUG
        #define CHG_LOG_TRACE
    #endif
#elif defined(CHG_LOG_ERROR)
#elif defined(CHG_LOG_WARN)
    #define CHG_LOG_ERROR
#elif defined(CHG_LOG_SUCCESS)
    #define CHG_LOG_ERROR
    #define CHG_LOG_WARN
#elif defined(CHG_LOG_INFO)
    #define CHG_LOG_ERROR
    #define CHG_LOG_WARN
    #define CHG_LOG_SUCCESS
#elif defined(CHG_LOG_DEBUG)
    #define CHG_LOG_ERROR
    #define CHG_LOG_WARN
    #define CHG_LOG_SUCCESS
    #define CHG_LOG_INFO
#elif defined(CHG_LOG_TRACE)
    #define CHG_LOG_ERROR
    #define CHG_LOG_WARN
    #define CHG_LOG_SUCCESS
    #define CHG_LOG_INFO
    #define CHG_LOG_DEBUG
#else
    #error No CHG_LOG defined.
#endif

//
// CHG_DELAYED_EVAL
//

#define CHG_DELAYED_EVAL(func) [&]() { return func; }

//
// LogType
//

enum class LogType : uint8_t
{
    NONE    = 0,
    FATAL   = 1 << 0,
    ERROR   = 1 << 1,
    WARN    = 1 << 2,
    SUCCESS = 1 << 3,
    INFO    = 1 << 4,
    DEBUG   = 1 << 5,
    TRACE   = 1 << 6,
    ALL     = 0xFF
};

//
// LogFormat
//

class LogFormat
{
private:
    const char*          format;
    std::source_location location;

public:
    LogFormat(const char* format, std::source_location&& location = std::source_location::current()) noexcept;
    LogFormat(const std::string& format, std::source_location&& location = std::source_location::current()) noexcept;
    LogFormat(const LogFormat&) = delete;
    LogFormat(LogFormat&&)      = default;

    const char*                 getFormat() const;
    const std::source_location& getLocation() const;
};

//
// Log
//

class LogBase
{
    class Settings
    {
    public:
        LogType            logLevel;
        FlagField<LogType> printNameLevel;
        FlagField<LogType> printAddressLevel;
        FlagField<LogType> printLocationLevel;
        std::regex         logSourceFilter;
        std::ostream&      outputStream;

        Settings(
            LogType            logLevel,
            FlagField<LogType> printNameLevel,
            FlagField<LogType> printAddressLevel,
            FlagField<LogType> printLocationLevel,
            std::regex&&       logSourceFilter,
            std::ostream&      outputStream) noexcept;
        Settings(const Settings&) = delete;
        Settings(Settings&&)      = default;
    };

private:
    const void*           address;
    const std::type_index sourceType;

    static uint8_t              contexts;
    static size_t               foldCount;
    static std::vector<LogType> hideStack;

    static void addContextIndent();

    std::string getSourceIdentifier(const std::source_location& location, const LogType type) const;

    template <class... Args>
    void log(const LogFormat& format, const LogType type, Args&&... args) const;

protected:
    void logFormatted(const std::string& msg, const std::source_location& location, const LogType type) const;

    LogBase(const void* address, const std::type_index sourceType) noexcept;
    LogBase(const LogBase&) = delete;
    LogBase(LogBase&&)      = delete;

public:
    template <class... Args>
    [[noreturn]] void fatal(const LogFormat& format, Args&&... args) const;
    template <class... Args>
    void error(const LogFormat& format, Args&&... args) const;
    template <class... Args>
    void warn(const LogFormat& format, Args&&... args) const;
    template <class... Args>
    void success(const LogFormat& format, Args&&... args) const;
    template <class... Args>
    void info(const LogFormat& format, Args&&... args) const;
    template <class... Args>
    void debug(const LogFormat& format, Args&&... args) const;
    template <class... Args>
    void trace(const LogFormat& format, Args&&... args) const;

    static void nest();
    static void unnest();

    static void hide(const LogType newLevel = LogType::NONE);
    static void unhide();

    static void breakline();

    static std::optional<LogType> parseLogType(const std::string& str);
    static bool                   isLogTypeEnabled(const LogType type);

    static Settings& settings();
};

template <class... Args>
void LogBase::log(const LogFormat& format, const LogType type, Args&&... args) const
{
    if (type > settings().logLevel && type != LogType::FATAL)
        return;

    // Check if log filter matches the source name, fatal logs are never ignored.
    // TODO: Demangling could be slow.
    if (type != LogType::FATAL &&
        settings().logSourceFilter.mark_count() &&
        sourceType != typeid(void) &&
        not std::regex_match(utils::demangleTypeName(sourceType.name()), settings().logSourceFilter))
        return;

    // Values returned from delayed-call args are temporary and need storage, normal args are stored
    // as references.
    const auto argStorage = std::make_tuple(utils::invokeOrForward(std::forward<Args>(args))...);
    const auto formatStr  = format.getFormat();

    const auto message = std::apply(
        [&](const auto&... pArgs) { return std::vformat(formatStr, std::make_format_args(pArgs...)); }, argStorage);

    logFormatted(message, format.getLocation(), type);
}

template <class... Args>
void LogBase::fatal(const LogFormat& format, Args&&... args) const
{
    log(std::move(format), LogType::FATAL, std::forward<Args>(args)...);

    OS::setTextColor(OS::BasicColor::DARK_RED, settings().outputStream);
    settings().outputStream << "Execution aborted due to a fatal error.\n";
    settings().outputStream.flush();
    OS::setTextColor(OS::BasicColor::WHITE, settings().outputStream);

    chg::fatal("Chemgine encountered a fatal error.");
}

template <class... Args>
void LogBase::error([[maybe_unused]] const LogFormat& format, [[maybe_unused]] Args&&... args) const
{
#ifdef CHG_LOG_ERROR
    log(std::move(format), LogType::ERROR, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::warn([[maybe_unused]] const LogFormat& format, [[maybe_unused]] Args&&... args) const
{
#ifdef CHG_LOG_WARN
    log(std::move(format), LogType::WARN, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::success([[maybe_unused]] const LogFormat& format, [[maybe_unused]] Args&&... args) const
{
#ifdef CHG_LOG_SUCCESS
    log(std::move(format), LogType::SUCCESS, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::info([[maybe_unused]] const LogFormat& format, [[maybe_unused]] Args&&... args) const
{
#ifdef CHG_LOG_INFO
    log(std::move(format), LogType::INFO, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::debug([[maybe_unused]] const LogFormat& format, [[maybe_unused]] Args&&... args) const
{
#ifdef CHG_LOG_DEBUG
    log(std::move(format), LogType::DEBUG, std::forward<Args>(args)...);
#endif
}

template <class... Args>
void LogBase::trace([[maybe_unused]] const LogFormat& format, [[maybe_unused]] Args&&... args) const
{
#ifdef CHG_LOG_TRACE
    log(std::move(format), LogType::TRACE, std::forward<Args>(args)...);
#endif
}

template <typename SourceT = void>
class Log : public LogBase
{
public:
    Log(const SourceT* address = nullptr) noexcept;
};

template <typename SourceT>
Log<SourceT>::Log(const SourceT* address) noexcept :
    LogBase(address, typeid(SourceT))
{}
