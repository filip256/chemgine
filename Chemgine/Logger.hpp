#pragma once

#include <string>
#include "LogType.hpp"


class Logger
{
public:
	static bool isEnabled;
	static void log(const char* str, const LogType type = LogType::NONE);
	static void log(const std::string& str, const LogType type = LogType::NONE);
	static void breakline();
};