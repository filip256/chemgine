#pragma once

#include <cstdint>

namespace OS
{
	namespace Color
	{
		// Windows color codes
		constexpr uint16_t Black = 0x0001;
		constexpr uint16_t DarkGrey = 0x0008;

		constexpr uint16_t Grey = 0x0004 | 0x0002 | 0x0001;
		constexpr uint16_t White = Grey | 0x0008;

		constexpr uint16_t DarkRed = 0x0004;
		constexpr uint16_t Red = DarkRed | 0x0008;

		constexpr uint16_t DarkGreen = 0x0002;
		constexpr uint16_t Green = DarkGreen | 0x0008;

		constexpr uint16_t DarkBlue = 0x0001;
		constexpr uint16_t Blue = DarkBlue | 0x0008;

		constexpr uint16_t DarkYellow = 0x0004 | 0x0002;
		constexpr uint16_t Yellow = DarkYellow | 0x0008;

		constexpr uint16_t DarkMagenta = 0x0004 | 0x0001;
		constexpr uint16_t Magenta = DarkMagenta | 0x0008;

		constexpr uint16_t DarkCyan = 0x0002 | 0x0001;
		constexpr uint16_t Cyan = DarkCyan | 0x0008;
	}

    void setTextColor(const uint16_t color);

	bool isRunningFromConsole();
}
