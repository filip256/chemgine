#pragma once

#include <string>

#if defined(_MSC_VER)
	#define CHG_UNREACHABLE() __assume(false)
#elif defined(__GNUC__) || defined(__clang__)
	#define CHG_UNREACHABLE() __builtin_unreachable()
#else
	#define CHG_UNREACHABLE() throw std::runtime_error("CHG_UNREACHABLE")
#endif

namespace Utils
{
	const std::string& getBuildTypeName();
}
