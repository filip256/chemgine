#include "Logger.hpp"
#include "LogType.hpp"

#include <Windows.h>

uint8_t Logger::contexts = 0;
LogType Logger::severityLevel = LogType::ALL;
std::ostream& Logger::outputStream = std::cout;
std::vector<std::pair<LogType, std::string>> Logger::cache;

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

	if (type > severityLevel)
		return;

	for (uint8_t i = 0; i < contexts; ++i)
		outputStream << "  ";

	if (type == LogType::BAD)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		outputStream << "ERROR:   ";
	}
	else if (type == LogType::WARN)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
		outputStream << "WARN:    ";
	}
	else if (type == LogType::INFO)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		outputStream << "INFO:    ";
	}
	else if (type == LogType::GOOD)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		outputStream << "SUCCESS: ";
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

void Logger::logCached(const char* str, const LogType type)
{
#ifdef CHEM_ENABLE_LOGGING
	if (type == LogType::FATAL)
	{
		fatal(str);
		return;
	}

	cache.emplace_back(std::make_pair(type, ""));
	for (uint8_t i = 0; i < contexts; ++i)
		cache.back().second += "  ";
	cache.back().second += str;
#endif
}

void Logger::logCached(const std::string& str, const LogType type)
{
#ifdef CHEM_ENABLE_LOGGING
	logCached(str.c_str(), type);
#endif
}

void Logger::printCache()
{
#ifdef CHEM_ENABLE_LOGGING
	for (size_t i = 0; i < cache.size(); ++i)
		Logger::log(cache[i].second, cache[i].first);
#endif
}

void Logger::clearCache()
{
#ifdef CHEM_ENABLE_LOGGING
	cache.clear();
#endif
}