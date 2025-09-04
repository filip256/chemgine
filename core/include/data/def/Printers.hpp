#pragma once

#include "data/def/Keywords.hpp"
#include "utils/Format.hpp"
#include "utils/Numeric.hpp"
#include "utils/STL.hpp"
#include "utils/String.hpp"

#include <format>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace def
{

template <typename T, typename... Args>
static std::string print(const T& object, Args&&... arg);
template <typename T, typename... Args>
static std::string prettyPrint(const T& object, Args&&... arg);

template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
static std::string printEnum(const E object);

template <typename T, typename = void>
class Printer
{
    static_assert(std::is_arithmetic_v<T>, "Printer: Unsupported type.");

public:
    static std::string print(const T object) { return std::to_string(object); }
};

template <typename T>
class Printer<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
public:
    static std::string print(const T object)
    {
        return utils::floatEqual(object, std::numeric_limits<T>::lowest()) ? std::string(def::Amounts::Min)
               : utils::floatEqual(object, std::numeric_limits<T>::max())
                   ? std::string(def::Amounts::Max)
                   : utils::formatFloatingPoint(std::format("{:.{}f}", object, 5));
    }
};

template <>
class Printer<std::string>
{
public:
    static std::string print(const std::string& str) { return utils::strip(str); }
};

template <>
class Printer<std::string_view>
{
public:
    static std::string print(const std::string_view str) { return def::print(std::string(str)); }
};

template <>
class Printer<bool>
{
public:
    static std::string print(const bool object) { return object ? "true" : "false"; }
};

template <typename T>
class Printer<T*>
{
public:
    static std::string print(const T* object) { return utils::toHex(object); }
};

template <typename T1, typename T2>
class Printer<std::pair<T1, T2>>
{
public:
    static std::string print(const std::pair<T1, T2>& object)
    {
        return '{' + def::print(object.first) + ':' + def::print(object.second) + '}';
    }

    static std::string prettyPrint(const std::pair<T1, T2>& object)
    {
        return "{ " + def::prettyPrint(object.first) + ": " + def::prettyPrint(object.second) + " }";
    }
};

template <typename T>
class Printer<std::vector<T>>
{
public:
    static std::string print(const std::vector<T>& object)
    {
        if (object.size() == 1)
            return def::print(object.front());

        std::string result = "{";
        for (size_t i = 0; i < object.size(); ++i)
            result += def::print(object[i]) + ',';

        result.back() = '}';
        return result;
    }

    static std::string prettyPrint(const std::vector<T>& object)
    {
        if (object.size() == 0)
            return "{}";

        if (object.size() == 1)
            return def::prettyPrint(*object.begin());

        std::string result = "{ ";
        for (size_t i = 0; i < object.size(); ++i)
            result += def::prettyPrint(object[i]) + ", ";

        result[result.size() - 2] = ' ';
        result.back()             = '}';
        return result;
    }
};

template <typename Key, typename T>
class Printer<std::unordered_map<Key, T>>
{
public:
    static std::string print(const std::unordered_map<Key, T>& object)
    {
        if (object.size() == 1) {
            const auto& p = *object.begin();
            return def::print(p.first) + ':' + def::print(p.second);
        }

        std::string result = "{";
        for (const auto& p : object)
            result += def::print(p.first) + ':' + def::print(p.second) + ',';

        result.back() = '}';
        return result;
    }

    static std::string prettyPrint(const std::unordered_map<Key, T>& object)
    {
        if (object.size() == 0)
            return "{}";

        if (object.size() == 1) {
            const auto& p = *object.begin();
            return def::prettyPrint(p.first) + ": " + def::prettyPrint(p.second);
        }

        std::string result = "{ ";
        for (const auto& p : object)
            result += def::prettyPrint(p.first) + ": " + def::prettyPrint(p.second) + ", ";

        result[result.size() - 2] = ' ';
        result.back()             = '}';
        return result;
    }
};

template <typename T>
class Printer<std::unordered_set<T>>
{
public:
    static std::string print(const std::unordered_set<T>& object)
    {
        if (object.size() == 1)
            return def::print(*object.begin());

        std::string result = "{";
        for (const auto& i : object)
            result += def::print(i) + ',';

        result.back() = '}';
        return result;
    }

    static std::string prettyPrint(const std::unordered_set<T>& object)
    {
        if (object.size() == 0)
            return "{}";

        if (object.size() == 1)
            return def::prettyPrint(*object.begin());

        std::string result = "{ ";
        for (const auto& i : object)
            result += def::prettyPrint(i) + ", ";

        result[result.size() - 2] = ' ';
        result.back()             = '}';
        return result;
    }
};

template <typename... Args>
class Printer<std::tuple<Args...>>
{
private:
    template <std::size_t... Is>
    static void printTupleElements(const std::tuple<Args...>& object, std::string& result, std::index_sequence<Is...>)
    {
        ((result += def::print(std::get<Is>(object)) + ','), ...);
    }

    template <std::size_t... Is>
    static void
    prettyPrintTupleElements(const std::tuple<Args...>& object, std::string& result, std::index_sequence<Is...>)
    {
        ((result += def::prettyPrint(std::get<Is>(object)) + ", "), ...);
    }

public:
    static std::string print(const std::tuple<Args...>& object)
    {
        if constexpr (sizeof...(Args) == 0)
            return "{}";
        else if constexpr (sizeof...(Args) == 1)
            return def::print(std::get<0>(object));
        else {
            std::string result = "{";
            printTupleElements(object, result, std::index_sequence_for<Args...>{});
            result.back() = '}';
            return result;
        }
    }

    static std::string prettyPrint(const std::tuple<Args...>& object)
    {
        if constexpr (sizeof...(Args) == 0)
            return "{}";
        else if constexpr (sizeof...(Args) == 1)
            return def::prettyPrint(std::get<0>(object));
        else {
            std::string result = "{ ";
            prettyPrintTupleElements(object, result, std::index_sequence_for<Args...>{});
            result[result.size() - 2] = ' ';
            result.back()             = '}';
            return result;
        }
    }
};

};  // namespace def

template <typename T, typename... Args>
inline std::string def::print(const T& object, Args&&... args)
{
    return def::Printer<T>::print(object, std::forward<Args>(args)...);
}

namespace
{

template <typename, typename = void>
struct has_pretty_print : std::false_type
{};

template <typename T>
struct has_pretty_print<T, std::void_t<decltype(&def::Printer<T>::prettyPrint)>> : std::true_type
{};

}  // namespace

template <typename T, typename... Args>
inline std::string def::prettyPrint(const T& object, Args&&... args)
{
    if constexpr (has_pretty_print<T>::value)
        return def::Printer<T>::prettyPrint(object, std::forward<Args>(args)...);
    else
        return def::Printer<T>::print(object, std::forward<Args>(args)...);
}

template <typename E, typename>
static std::string def::printEnum(const E object)
{
    return std::to_string(static_cast<std::underlying_type_t<E>>(object));
}
