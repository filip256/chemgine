#pragma once

#include <format>
#include <stdexcept>
#include <iostream>

#define CHG_THROW(format, ...)                                                          \
{                                                                                       \
    std::cerr << std::vformat(format, std::make_format_args(__VA_ARGS__)) << '\n';      \
    throw std::runtime_error(std::vformat(format, std::make_format_args(__VA_ARGS__))); \
}
