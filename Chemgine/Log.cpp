#include "Log.hpp"
#include "LogType.hpp"

#include <Windows.h>

uint8_t LogBase::contexts = 0;
std::vector<std::string> LogBase::cache;

LogBase::LogBase(const void* location) noexcept :
	location(location)
{}

std::string LogBase::getAddress() const
{
	return location ? Linguistics::toHex(location) : "";
}

void LogBase::nest()
{
	if (contexts < 255)
		++contexts;
}

void LogBase::unnest()
{
	if(contexts > 0)
		--contexts;
}

void LogBase::breakline()
{
	outputStream << "\n.........................................................................\n\n";
}

void LogBase::clearCache()
{
	cache.clear();
}
