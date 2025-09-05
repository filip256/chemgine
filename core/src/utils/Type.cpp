#include "utils/Type.hpp"

#include "utils/String.hpp"

void utils::demangleTypeName(std::string& name)
{
    if (const auto idx = name.rfind("::"); idx != std::string::npos)
        name = name.substr(idx + 2);
    if (name.starts_with("class"))
        name = name.substr(5);

    utils::strip(name);
}

std::string utils::demangleTypeName(const std::string& name)
{
    std::string temp = name;
    demangleTypeName(temp);
    return temp;
}
