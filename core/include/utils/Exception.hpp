#pragma once

#include <format>
#include <iostream>
#include <stdexcept>

namespace chg
{

template <typename... Args>
[[noreturn]] void fatal(std::string_view format, Args&&... args)
{
    // TODO: This is inneficient, maybe only store const references / temporaries.
    auto       lvalues = std::make_tuple(std::forward<Args>(args)...);
    const auto formatted =
        std::apply([&](auto&... vals) { return std::vformat(format, std::make_format_args(vals...)); }, lvalues);

    std::cerr << formatted << '\n';
    throw std::runtime_error(formatted);
}

}  // namespace chg
