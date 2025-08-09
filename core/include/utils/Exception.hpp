#pragma once

#include <format>
#include <stdexcept>
#include <iostream>

namespace chg
{

template<typename... Args>
[[noreturn]] void fatal(std::string_view format, Args&&... args)
{
#ifdef _MSC_VER
    // MSVC's make_format_args expects non-const lvalue references.
    // TODO: This is inneficient, maybe only store const references / temporaries.
    auto lvalues = std::make_tuple(std::forward<Args>(args)...);
    const auto formatted = std::apply(
        [&](auto&... vals) {
            return std::vformat(format, std::make_format_args(vals...));
        },
        lvalues
    );
#else
    const auto formatted = std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
#endif

    std::cerr << formatted << '\n';
    throw std::runtime_error(formatted);
}

} // namespace chg
