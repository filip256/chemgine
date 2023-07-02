#pragma once

#include <string>
#include <iostream>
#include "LogType.hpp"

#ifndef NDEBUG
#define CHEM_ENABLE_LOGGING
#endif

class Logger
{
private:
	static uint8_t contexts;
public:
	static LogType severityLevel;
	static std::ostream& outputStream;
	static void enterContext();
	static void exitContext();
	static void log(const char* str, const LogType type = LogType::NONE);
	static void log(const std::string& str, const LogType type = LogType::NONE);
	static void fatal(const char* str);
	static void fatal(const std::string& str);
	static void breakline();
};