#pragma once

#include "utils/STL.hpp"

#include <cstdint>

namespace OS
{
	typedef uint16_t ColorType;

	namespace Color
	{
		// Windows color codes:
		// https://learn.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
		constexpr ColorType Black = 0x0000;

		constexpr ColorType DarkGrey = 0x0008;
		constexpr ColorType Grey = 0x0004 | 0x0002 | 0x0001;
		constexpr ColorType White = Grey | 0x0008;

		constexpr ColorType DarkRed = 0x0004;
		constexpr ColorType Red = DarkRed | 0x0008;

		constexpr ColorType DarkGreen = 0x0002;
		constexpr ColorType Green = DarkGreen | 0x0008;

		constexpr ColorType DarkBlue = 0x0001;
		constexpr ColorType Blue = DarkBlue | 0x0008;

		constexpr ColorType DarkYellow = 0x0004 | 0x0002;
		constexpr ColorType Yellow = DarkYellow | 0x0008;

		constexpr ColorType DarkMagenta = 0x0004 | 0x0001;
		constexpr ColorType Magenta = DarkMagenta | 0x0008;

		constexpr ColorType DarkCyan = 0x0002 | 0x0001;
		constexpr ColorType Cyan = DarkCyan | 0x0008;

		constexpr ColorType DarkGreyBG = 0x0080;
		constexpr ColorType GreyBG = 0x0040 | 0x0020 | 0x0010;
		constexpr ColorType WhiteBG = GreyBG | 0x0080;
							
		constexpr ColorType DarkRedBG = 0x0040;
		constexpr ColorType RedBG = DarkRedBG | 0x0080;
							
		constexpr ColorType DarkGreenBG = 0x0020;
		constexpr ColorType GreenBG = DarkGreenBG | 0x0080;
							
		constexpr ColorType DarkBlueBG = 0x0010;
		constexpr ColorType BlueBG = DarkBlueBG | 0x0080;
							
		constexpr ColorType DarkYellowBG = 0x0040 | 0x0020;
		constexpr ColorType YellowBG = DarkYellowBG | 0x0080;
							
		constexpr ColorType DarkMagentaBG = 0x0040 | 0x0010;
		constexpr ColorType MagentaBG = DarkMagentaBG | 0x0080;
							
		constexpr ColorType DarkCyanBG = 0x0020 | 0x0010;
		constexpr ColorType CyanBG = DarkCyanBG | 0x0080;

		constexpr ColorType None = utils::npos<ColorType>;
	}

	constexpr ColorType brighten(const ColorType color);
	constexpr ColorType darken(const ColorType color);

	ColorType getTextColor();
    void setTextColor(const ColorType color);

	bool isRunningFromConsole();
}


constexpr OS::ColorType OS::brighten(const ColorType color)
{
	return color | 0x0008 | 0x0080;
}

constexpr OS::ColorType OS::darken(const ColorType color)
{
	return color & ~0x0008 & ~0x0080;
}
