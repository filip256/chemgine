#include "Logger.hpp"
#include "LogType.hpp"

#include <Windows.h>

uint8_t Logger::contexts = 0;
std::ostream& Logger::outputStream = std::cout;

void Logger::enterContext()
{
#ifdef CHEM_ENABLE_LOGGING
	if (contexts < 255)
		++contexts;
#endif
}

void Logger::exitContext()
{
#ifdef CHEM_ENABLE_LOGGING
	if(contexts > 0)
		--contexts;
#endif
}

void Logger::log(const char* str, const LogType type)
{
#ifdef CHEM_ENABLE_LOGGING
	if (type == LogType::FATAL)
	{
		fatal(str);
		return;
	}

	for (uint8_t i = 0; i < contexts; ++i)
		outputStream << "  ";

	switch (type)
	{
	case LogType::BAD:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		outputStream << "ERROR:   ";
		break;

	case LogType::WARN:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
		outputStream << "WARN:    ";
		break;

	case LogType::GOOD:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		outputStream << "SUCCESS: ";
		break;
	}

	if(type != LogType::NONE)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

	outputStream << str << '\n';
#endif
}

void Logger::log(const std::string& str, const LogType type)
{
#ifdef CHEM_ENABLE_LOGGING
	log(str.c_str(), type);
#endif
}

void Logger::fatal(const char* str)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	outputStream << "FATAL:   ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	outputStream << str << '\n';
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	outputStream << "\n   The execution was halted due to a fatal error!\n   Press ENTER to exit.\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	getchar();
	std::exit(EXIT_FAILURE);
}

void Logger::fatal(const std::string& str)
{
	fatal(str.c_str());
}

void Logger::breakline()
{
#ifdef CHEM_ENABLE_LOGGING
	outputStream << "\n.........................................................................\n\n";
#endif
}