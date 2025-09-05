#pragma once

#include <cstdint>
#include <ostream>

namespace OS
{

enum class BasicColor : uint8_t
{
    NONE,

    BLACK,
    WHITE,

    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN,
    GREY,

    DARK_RED,
    DARK_GREEN,
    DARK_BLUE,
    DARK_YELLOW,
    DARK_MAGENTA,
    DARK_CYAN,
    DARK_GREY,

    BLACK_BG,
    WHITE_BG,

    RED_BG,
    GREEN_BG,
    BLUE_BG,
    YELLOW_BG,
    MAGENTA_BG,
    CYAN_BG,
    GREY_BG,

    DARK_RED_BG,
    DARK_GREEN_BG,
    DARK_BLUE_BG,
    DARK_YELLOW_BG,
    DARK_MAGENTA_BG,
    DARK_CYAN_BG,
    DARK_GREY_BG,
};

BasicColor brighten(const BasicColor color);
BasicColor darken(const BasicColor color);

BasicColor getTextColor();
void       setTextColor(const BasicColor color, std::ostream& os);

}  // namespace OS
