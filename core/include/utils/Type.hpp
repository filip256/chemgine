#pragma once

#include <string>
#include <typeinfo>

namespace utils
{

void        demangleTypeName(std::string& name);
std::string demangleTypeName(const std::string& name);

template <typename T>
std::string getTypeName();

}  // namespace utils

template <typename T>
std::string utils::getTypeName()
{
    if constexpr (std::is_same<T, void>())
        return "";
    else {
        auto name = std::string(typeid(T).name());
        demangleTypeName(name);
        return name;
    }
}
