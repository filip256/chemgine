#pragma once

#include "data/def/Keywords.hpp"
#include "utils/STL.hpp"
#include "utils/String.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace def
{

template <typename T, typename... Args>
static std::optional<T> parse(const std::string& str, Args&&... args);
template <typename T>
static std::optional<T> parseUnsigned(const std::string& str);

template <typename E>
static std::optional<E> parseEnum(const std::string& str);

template <typename T, typename = void>
class Parser
{
    static_assert(std::is_arithmetic_v<T>, "Parser: Unsupported type.");

public:
    static std::optional<T> parse(const std::string& str)
    {
        if constexpr (std::is_integral_v<T>) {
            const auto r = def::parse<int64_t>(str);
            return r && *r >= std::numeric_limits<T>::min() && *r <= std::numeric_limits<T>::max()
                       ? std::optional(static_cast<T>(*r))
                       : std::nullopt;
        }
        else if constexpr (std::is_floating_point_v<T>) {
            const auto stripped = utils::strip(str);
            if (stripped.empty())
                return std::nullopt;

            if (stripped == def::Amounts::Min)
                return std::numeric_limits<T>::lowest();
            if (stripped == def::Amounts::Max)
                return std::numeric_limits<T>::max();

            try {
                size_t     pos = 0;
                const auto val = std::stold(stripped, &pos);
                return pos == str.size() &&
                               val >= std::numeric_limits<T>::lowest() &&
                               val <= std::numeric_limits<T>::max()
                           ? std::optional(static_cast<T>(val))
                           : std::nullopt;
            } catch (const std::invalid_argument&) {
                return std::nullopt;
            } catch (const std::out_of_range&) {
                return std::nullopt;
            }
        }
    }
};

template <>
class Parser<std::string>
{
public:
    static std::optional<std::string> parse(const std::string& str) { return utils::strip(str); }
};

template <>
class Parser<int64_t>
{
public:
    static std::optional<int64_t> parse(const std::string& str)
    {
        if (str.empty())
            return std::nullopt;

        try {
            size_t     pos = 0;
            const auto val = std::stoll(str, &pos);
            return pos == str.size() ? std::optional(val) : std::nullopt;
        } catch (const std::invalid_argument&) {
            return std::nullopt;
        } catch (const std::out_of_range&) {
            return std::nullopt;
        }
    }
};

template <>
class Parser<uint64_t>
{
public:
    static std::optional<uint64_t> parse(const std::string& str)
    {
        if (str.empty())
            return std::nullopt;

        try {
            size_t     pos = 0;
            const auto val = std::stoull(str, &pos);
            return pos == str.size() ? std::optional(val) : std::nullopt;
        } catch (const std::invalid_argument&) {
            return std::nullopt;
        } catch (const std::out_of_range&) {
            return std::nullopt;
        }
    }
};

template <>
class Parser<bool>
{
public:
    static std::optional<bool> parse(const std::string& str)
    {
        if (str.empty())
            return std::nullopt;

        if (str == "true")
            return std::optional(true);
        if (str == "false")
            return std::optional(false);

        return std::nullopt;
    }
};

template <typename T>
class Parser<std::vector<T>>
{
private:
    static std::string removeBrackets(const std::string& str)
    {
        return str.starts_with('{') && str.ends_with('}') ? str.substr(1, str.size() - 2) : str;
    }

public:
    static std::optional<std::vector<T>> parse(const std::string& str, const bool ignoreEmpty = true)
    {
        const auto listStr = removeBrackets(str);

        std::vector<T> result;
        int64_t        lastSep        = -1;
        size_t         ignoreSections = 0;

        for (size_t i = 0; i < listStr.size(); ++i) {
            if (listStr[i] == '{') {
                ++ignoreSections;
                continue;
            }
            if (listStr[i] == '}' && ignoreSections > 0) {
                --ignoreSections;
                continue;
            }

            if (ignoreSections != 0)
                continue;

            if (listStr[i] != ',')
                continue;

            if (ignoreEmpty == false || i - lastSep - 1 > 0) {
                auto r = def::parse<T>(utils::strip(
                    listStr.substr(static_cast<size_t>(lastSep + 1), static_cast<size_t>(i - lastSep - 1))));
                if (not r)
                    return std::nullopt;

                result.emplace_back(std::move(*r));
            }
            lastSep = i;
        }

        if (ignoreEmpty == false || static_cast<size_t>(lastSep + 1) < listStr.size()) {
            auto r = def::parse<T>(utils::strip(listStr.substr(static_cast<size_t>(lastSep + 1))));
            if (not r)
                return std::nullopt;

            result.emplace_back(std::move(*r));
        }
        return result;
    }
};

template <typename T1, typename T2>
class Parser<std::pair<T1, T2>>
{
private:
    static std::string removeBrackets(const std::string& str)
    {
        return str.starts_with('{') && str.ends_with('}') ? str.substr(1, str.size() - 2) : str;
    }

public:
    static std::optional<std::pair<T1, T2>> parse(const std::string& str, const char sep = ':')
    {
        const auto& pairStr = utils::split(removeBrackets(utils::strip(str)), sep, '{', '}', true);
        if (pairStr.size() != 2)
            return std::nullopt;

        const auto val1 = def::parse<T1>(utils::strip(pairStr.front()));
        if (not val1)
            return std::nullopt;

        const auto val2 = def::parse<T2>(utils::strip(pairStr.back()));
        if (not val2)
            return std::nullopt;

        return std::optional<std::pair<T1, T2>>(std::make_pair(*val1, *val2));
    }
};

template <typename T>
class Parser<std::unordered_map<std::string, T>>
{
public:
    static std::optional<std::unordered_map<std::string, T>> parse(const std::string& str)
    {
        auto entries = def::parse<std::vector<std::pair<std::string, std::string>>>(str);
        if (not entries)
            return std::nullopt;

        std::unordered_map<std::string, T> result;
        result.reserve(entries->size());
        for (size_t i = 0; i < entries->size(); ++i) {
            utils::strip((*entries)[i].second);
            auto value = def::parse<T>((*entries)[i].second);
            if (not value)
                return std::nullopt;

            result.emplace(std::move((*entries)[i].first), *std::move(value));
        }

        return result;
    }
};

};  // namespace def

template <typename T, typename... Args>
inline std::optional<T> def::parse(const std::string& str, Args&&... args)
{
    return def::Parser<T>::parse(str, std::forward<Args>(args)...);
}

template <typename T>
inline std::optional<T> def::parseUnsigned(const std::string& str)
{
    const auto r = def::Parser<T>::parse(str);
    return not r || *r < 0 ? std::nullopt : std::optional(r);
}

template <typename E>
inline std::optional<E> def::parseEnum(const std::string& str)
{
    static_assert(std::is_enum_v<E>, "Enum Parser: Unsupported non-enum type.");

    const auto val = parse<std::underlying_type_t<E>>(str);
    return val ? std::make_optional(static_cast<E>(*val)) : std::nullopt;
}
