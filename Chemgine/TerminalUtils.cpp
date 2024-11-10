#include "TerminalUtils.hpp"

#include <Windows.h>

namespace OS
{
	void setTextColor(const uint16_t color)
	{
		static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, color);
	}
}
