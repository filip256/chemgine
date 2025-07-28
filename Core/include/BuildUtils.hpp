#pragma once

#include <string>

#include "ExceptionUtils.hpp"

#if defined(_MSC_VER)
	#define CHG_UNREACHABLE() __assume(false)
#elif defined(__GNUC__) || defined(__clang__)
	#define CHG_UNREACHABLE() __builtin_unreachable()
#else
	#define CHG_UNREACHABLE() CHG_THROW("Reached CHG_UNREACHABLE.")
#endif

namespace utils
{
	const std::string& getBuildTypeName();
}
