#pragma once

#include "utils/Exception.hpp"

#include <string>

// CHG_UNREACHABLE
#if defined(_MSC_VER)
    #define CHG_UNREACHABLE() __assume(false)
#elif defined(__GNUC__) || defined(__clang__)
    #define CHG_UNREACHABLE() __builtin_unreachable()
#else
    #define CHG_UNREACHABLE() \
        do {                  \
        } while (0)
#endif

// CHG_BUILD_...
#if defined(_WIN32) || defined(_WIN64)
    #define CHG_BUILD_WINDOWS
#elif defined(__linux__)
    #define CHG_BUILD_LINUX
#else
static_assert("Unsupported target operating system.");
#endif

namespace utils
{

const std::string& getBuildTypeName();

}  // namespace utils
