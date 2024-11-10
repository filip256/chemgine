#include "Log.hpp"
#include "LogType.hpp"

#include <algorithm>

uint8_t LogBase::contexts = 0;
size_t LogBase::foldCount = static_cast<size_t>(-1);
std::vector<LogType> LogBase::hideStack = {};

LogBase::LogBase(const void* location) noexcept :
	location(location)
{}

std::string LogBase::getAddress() const
{
	return location ? Linguistics::toHex(location) : "";
}

void LogBase::addContextIndent()
{
	OS::setTextColor(OS::Color::DarkGrey);
	for (uint8_t i = 0; i < contexts; ++i)
		outputStream << contexIndent;
	OS::setTextColor(OS::Color::White);
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

void LogBase::hide(const LogType newLevel)
{
	const auto level = std::min(logLevel, newLevel);

	hideStack.emplace_back(logLevel);
	logLevel = level;
}

void LogBase::unhide()
{
	if (hideStack.empty())
		return;

	logLevel = hideStack.back();
	hideStack.pop_back();
}

void LogBase::breakline()
{
	outputStream << "\n.........................................................................\n\n";
}
