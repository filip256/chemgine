#include "Logger.hpp"
#include "LogType.hpp"

#include <iostream>
#include <Windows.h>

bool Logger::isEnabled = true;

void Logger::log(const char* str, const LogType type)
{
	if (isEnabled == false)
		return;

	switch (type)
	{
	case LogType::FATAL:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		std::cout << "FATAL:   ";
		break;

	case LogType::BAD:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "ERROR:   ";
		break;

	case LogType::WARN:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
		std::cout << "WARN:    ";
		break;

	case LogType::GOOD:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		std::cout << "SUCCESS: ";
		break;
	}

	if(type != LogType::NONE)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

	std::cout << str << '\n';

	if (type == LogType::FATAL)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		std::cout << "\n   The execution was halted due to a fatal error!\n   Press ENTER to exit.\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		getchar();
		std::exit(EXIT_FAILURE);
	}
}

void Logger::log(const std::string& str, const LogType type)
{
	log(str.c_str(), type);
}

void Logger::breakline()
{
	std::cout << "\n.........................................................................\n\n";
}