#pragma once

#include <cstdint>

enum class LogType : uint8_t
{
	NONE,
	FATAL,
	ERROR,
	WARN,
	SUCCESS,
	INFO,
	DEBUG,
	TRACE,
	TABLE,
	ALL
};
