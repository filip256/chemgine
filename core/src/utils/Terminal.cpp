#include "utils/Terminal.hpp"

#include "utils/Casts.hpp"
#include "utils/Concurrency.hpp"
#include "utils/Build.hpp"

#ifdef CHG_BUILD_WINDOWS
	#include <Windows.h>
#endif

#ifdef CHG_BUILD_WINDOWS

namespace
{
	HANDLE getStdOutputHandle()
	{
		const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if(handle == INVALID_HANDLE_VALUE)
			chg::fatal("Failed to to retrieve STD_OUTPUT_HANDLE (error code: {0}).", GetLastError());
		return handle;
	}

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

#endif

OS::ColorType OS::getTextColor()
{
#ifdef CHG_BUILD_WINDOWS
	static const auto handle = getStdOutputHandle();

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (not GetConsoleScreenBufferInfo(handle, &info))
		chg::fatal("Failed to to get console screen buffer info (error code: {0}).", GetLastError());

	return checked_cast<ColorType>(info.wAttributes);
#else
	return Color::White;
#endif
}

void OS::setTextColor(const ColorType color)
{
#ifdef CHG_BUILD_WINDOWS
	std::cerr << "HERE 7\n";
	// Changing the text color is expected to always be followed by a text output procedure.
	// A locking mechanism is needed at the call site to ensure the text has the desired color, not here.
	CHG_NEVER_CONCURRENT();

	if (color == Color::None)
		return;

	static const auto handle = getStdOutputHandle();
	static auto currentColor = getTextColor();

	std::cerr << "HERE 8\n";
	if (color == currentColor)
		return;

	currentColor = color;
	if(not SetConsoleTextAttribute(handle, color))
		chg::fatal("Failed to to set console text attribute (error code: {0}).", GetLastError());
	std::cerr << "HERE 9\n";
#endif
}

bool OS::isRunningFromConsole()
{
#ifdef CHG_BUILD_WINDOWS
	static const auto fromConsole = _isRunningFromConsole();
	return fromConsole;
#else
	return true;
#endif
}
