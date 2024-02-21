#pragma once

#include <cstdint>

enum class LogType : uint8_t
{
	NONE,
	FATAL,
	BAD,
	WARN,
	GOOD,
	INFO,
	TABLE,
	ALL
};