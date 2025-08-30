#pragma once

#include "utils/Casts.hpp"
#include "utils/Exception.hpp"

#include <chrono>
#include <string>

namespace utils
{

void        pluralize(std::string& str);
std::string pluralize(const std::string& str);

void        capitalize(std::string& str);
std::string capitalize(const std::string& str);

void        formatFloatingPoint(std::string& str, const uint8_t maxDigits = 255);
std::string formatFloatingPoint(const std::string& str, const uint8_t maxDigits = 255);

template <typename T, typename = std::enable_if<std::is_floating_point_v<T>>>
std::string formatFloatingPoint(const T& value, const uint8_t maxDigits = 255);

void        padLeft(std::string& str, const size_t minLength, const char padding);
std::string padLeft(const std::string& str, const size_t minLength, const char padding);
void        padRight(std::string& str, const size_t minLength, const char padding);
std::string padRight(const std::string& str, const size_t minLength, const char padding);

enum class TimeFormat
{
    DIGITAL_HH_MM_SS,
    HUMAN_HH_MM_SS,
};

template <typename TimeT>
std::string formatTime(TimeT time, const TimeFormat format);

std::string toHex(const uint64_t value);
std::string toHex(const void* ptr);

template <uint8_t B, typename T>
std::string toBase(T value);
template <uint8_t B, typename T>
uint8_t getDigitCount(const T value);

};  // namespace utils

template <typename T, typename>
std::string utils::formatFloatingPoint(const T& value, const uint8_t maxDigits)
{
    return formatFloatingPoint(std::to_string(value), maxDigits);
}

template <typename TimeT>
std::string utils::formatTime(TimeT time, const TimeFormat format)
{
    const auto h  = std::chrono::duration_cast<std::chrono::hours>(time);
    time         -= h;
    const auto m  = std::chrono::duration_cast<std::chrono::minutes>(time);
    time         -= m;
    const auto s  = std::chrono::duration_cast<std::chrono::seconds>(time);

    switch (format) {
    case TimeFormat::DIGITAL_HH_MM_SS:
        return std::to_string(h.count()) +
               ':' +
               padLeft(std::to_string(m.count()), 2, '0') +
               ':' +
               padLeft(std::to_string(s.count()), 2, '0');

    case TimeFormat::HUMAN_HH_MM_SS:
        {
            std::string result;
            if (h.count() > 0) {
                result += std::to_string(h.count()) + 'h';
            }
            if (m.count() > 0) {
                result += std::to_string(m.count()) + 'm';
            }
            if (s.count() > 0) {
                result += std::to_string(s.count()) + 's';
            }

            return result.size() ? result : "0s";
        }

    default:
        chg::fatal("Unknown time format: {0}.", underlying_cast(format));
        return "";
    }
}

template <uint8_t B, typename T>
std::string utils::toBase(T value)
{
    static constexpr std::string_view digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    static_assert(std::is_integral_v<T>, "toBase(): Input type must be an integral type.");
    static_assert(B >= 2 && B <= digits.size(), "toBase(): Invalid or unsupported base.");

    if (value == 0)
        return "0";

    std::string result;

    while (value > 0) {
        result += digits[value % B];
        value  /= B;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

template <uint8_t B, typename T>
uint8_t utils::getDigitCount(const T value)
{
    static_assert(std::is_integral_v<T>, "getDigitCount(): Input type must be an integral type.");

    if (value == 0)
        return 1;

    // log_B(n) = log_X(n) / log_X(B)
    static const auto bLog = std::log(B);
    return static_cast<uint8_t>(std::log(value) / bLog) + 1;
}
