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

// CHG_BUILD_{OS}
#if defined(_WIN32) || defined(_WIN64)
    #define CHG_BUILD_WINDOWS
#elif defined(__linux__)
    #define CHG_BUILD_LINUX
#else
static_assert("Unsupported target operating system.");
#endif

// CHG_{OS}_ONLY
#ifdef CHG_BUILD_WINDOWS
    #define CHG_WINDOWS_ONLY(...) __VA_ARGS__
#else
    #define CHG_WINDOWS_ONLY(...)
#endif

#ifdef CHG_BUILD_LINUX
    #define CHG_LINUX_ONLY(...) __VA_ARGS__
#else
    #define CHG_LINUX_ONLY(...)
#endif

namespace utils
{

const std::string& getBuildTypeName();

}  // namespace utils
