#include "utils/Type.hpp"

#include "utils/String.hpp"

#include <algorithm>

void utils::demangleTypeName(std::string& name)
{
    if (const auto idx = name.rfind("::"); idx != std::string::npos)
        name = name.substr(idx + 2);
    if (name.starts_with("class"))
        name = name.substr(5);

    utils::strip(name, [](const auto c) { return isWhiteSpace(c) || c == '_'; });
    std::ranges::replace(name, ' ', '-');
}

std::string utils::demangleTypeName(const std::string& name)
{
    std::string temp = name;
    demangleTypeName(temp);
    return temp;
}
