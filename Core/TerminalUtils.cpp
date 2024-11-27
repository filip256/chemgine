#include "TerminalUtils.hpp"

#include <Windows.h>

void OS::setTextColor(const uint16_t color)
{
	static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, color);
}

namespace
{
	bool _isRunningFromConsole()
	{
		const auto consoleHwnd = GetConsoleWindow();
		if (consoleHwnd == nullptr)
			return false;

		DWORD dwProcessId;
		GetWindowThreadProcessId(consoleHwnd, &dwProcessId);
		return GetCurrentProcessId() != dwProcessId;
	}
}

bool OS::isRunningFromConsole()
{
	static const auto fromConsole = _isRunningFromConsole();
	return fromConsole;
}
