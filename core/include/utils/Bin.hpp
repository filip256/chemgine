#pragma once

#include "utils/Casts.hpp"

#include <istream>
#include <optional>
#include <ostream>

namespace bin
{

template <typename T, typename... Args>
std::enable_if_t<!std::is_enum_v<T>> print(std::ostream& os, const T& value, Args&&... args);
template <typename T, typename... Args>
std::enable_if_t<!std::is_enum_v<T>, std::optional<T>> parse(std::istream& is, Args&&... args);

template <typename T, typename... Args>
std::enable_if_t<std::is_enum_v<T>> print(std::ostream& os, const T& value, Args&&... args);
template <typename T, typename... Args>
std::enable_if_t<std::is_enum_v<T>, std::optional<T>> parse(std::istream& is, Args&&... args);

namespace details
{

template <typename T, typename = void>
class Formatter
{
    static_assert(std::is_arithmetic_v<T>, "Formatter: Unsupported type.");

public:
    static void print(std::ostream& os, const T& value) { os.write(reinterpret_cast<const char*>(&value), sizeof(T)); }

    static std::optional<T> parse(std::istream& is)
    {
        T value;
        is.read(reinterpret_cast<char*>(&value), sizeof(T));
        return is ? std::optional(value) : std::nullopt;
    }
};

template <typename T1, typename T2>
class Formatter<std::pair<T1, T2>>
{
public:
    static void print(std::ostream& os, const std::pair<T1, T2>& pair)
    {
        bin::print(os, pair.first);
        bin::print(os, pair.second);
    }

    static std::optional<std::pair<T1, T2>> parse(std::istream& is)
    {
        const auto first = bin::parse<T1>(is);
        if (not first)
            return std::nullopt;
        const auto second = bin::parse<T2>(is);
        if (not second)
            return std::nullopt;

        return std::make_pair(*first, *second);
    }
};

}  // namespace details
}  // namespace bin

namespace utils
{

std::string toBin(const std::string& str, const char byteDelim = '-', const char octetDelim = '\0');
std::string toHex(const std::string& str, const char byteDelim = '-', const char octetDelim = '\0');

}  // namespace utils

template <typename T, typename... Args>
std::enable_if_t<!std::is_enum_v<T>> bin::print(std::ostream& os, const T& value, Args&&... args)
{
    details::Formatter<T>::print(os, value, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
std::enable_if_t<!std::is_enum_v<T>, std::optional<T>> bin::parse(std::istream& is, Args&&... args)
{
    return details::Formatter<T>::parse(is, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
std::enable_if_t<std::is_enum_v<T>> bin::print(std::ostream& os, const T& value, Args&&... args)
{
    print(os, underlying_cast(value), std::forward<Args>(args)...);
}

template <typename T, typename... Args>
std::enable_if_t<std::is_enum_v<T>, std::optional<T>> bin::parse(std::istream& is, Args&&... args)
{
    auto raw = parse<std::underlying_type_t<T>>(is, std::forward<Args>(args)...);
    return raw ? std::optional(static_cast<T>(*raw)) : std::nullopt;
}
