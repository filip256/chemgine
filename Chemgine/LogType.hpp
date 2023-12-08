#pragma once

#include <cstdint>

enum class LogType : uint8_t
{
	NONE = 0,
	FATAL = 1,
	BAD = 2,
	WARN = 3,
	GOOD = 4
};