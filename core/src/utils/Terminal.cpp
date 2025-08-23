#include "utils/Terminal.hpp"

#include "utils/Build.hpp"
#include "utils/Casts.hpp"
#include "utils/Concurrency.hpp"
#include "utils/STL.hpp"

#include <bitset>

#if defined(CHG_COLOR_PRINT_MODE_WINAPI) && not defined(CHG_BUILD_WINDOWS)
    #error Color printing mode: 'WINAPI' is only supported for Windows builds.
#endif

#if defined(CHG_BUILD_WINDOWS)
    #include <Windows.h>

namespace Windows
{

enum class BasicColor : uint16_t
{
    NONE = utils::npos<uint16_t>,

    // Windows color codes:
    // https://learn.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
    BLACK = 0x0000,
    WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,

    RED     = FOREGROUND_RED | FOREGROUND_INTENSITY,
    GREEN   = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    BLUE    = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    YELLOW  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    CYAN    = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    GREY    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

    DARK_RED     = FOREGROUND_RED,
    DARK_GREEN   = FOREGROUND_GREEN,
    DARK_BLUE    = FOREGROUND_BLUE,
    DARK_YELLOW  = FOREGROUND_RED | FOREGROUND_GREEN,
    DARK_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
    DARK_CYAN    = FOREGROUND_GREEN | FOREGROUND_BLUE,
    DARK_GREY    = FOREGROUND_INTENSITY,

    BLACK_BG = 0x0000,
    WHITE_BG = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,

    RED_BG     = BACKGROUND_RED | BACKGROUND_INTENSITY,
    GREEN_BG   = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
    BLUE_BG    = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    YELLOW_BG  = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
    MAGENTA_BG = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    CYAN_BG    = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    GREY_BG    = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,

    DARK_RED_BG     = BACKGROUND_RED,
    DARK_GREEN_BG   = BACKGROUND_GREEN,
    DARK_BLUE_BG    = BACKGROUND_BLUE,
    DARK_YELLOW_BG  = BACKGROUND_RED | BACKGROUND_GREEN,
    DARK_MAGENTA_BG = BACKGROUND_RED | BACKGROUND_BLUE,
    DARK_CYAN_BG    = BACKGROUND_GREEN | BACKGROUND_BLUE,
    DARK_GREY_BG    = BACKGROUND_INTENSITY,
};

BasicColor colorCast(const OS::BasicColor color)
{
    switch (color) {
    case OS::BasicColor::NONE:
        return BasicColor::NONE;
    case OS::BasicColor::BLACK:
        return BasicColor::BLACK;
    case OS::BasicColor::WHITE:
        return BasicColor::WHITE;
    case OS::BasicColor::RED:
        return BasicColor::RED;
    case OS::BasicColor::GREEN:
        return BasicColor::GREEN;
    case OS::BasicColor::BLUE:
        return BasicColor::BLUE;
    case OS::BasicColor::YELLOW:
        return BasicColor::YELLOW;
    case OS::BasicColor::MAGENTA:
        return BasicColor::MAGENTA;
    case OS::BasicColor::CYAN:
        return BasicColor::CYAN;
    case OS::BasicColor::GREY:
        return BasicColor::GREY;
    case OS::BasicColor::DARK_RED:
        return BasicColor::DARK_RED;
    case OS::BasicColor::DARK_GREEN:
        return BasicColor::DARK_GREEN;
    case OS::BasicColor::DARK_BLUE:
        return BasicColor::DARK_BLUE;
    case OS::BasicColor::DARK_YELLOW:
        return BasicColor::DARK_YELLOW;
    case OS::BasicColor::DARK_MAGENTA:
        return BasicColor::DARK_MAGENTA;
    case OS::BasicColor::DARK_CYAN:
        return BasicColor::DARK_CYAN;
    case OS::BasicColor::DARK_GREY:
        return BasicColor::DARK_GREY;
    case OS::BasicColor::BLACK_BG:
        return BasicColor::BLACK_BG;
    case OS::BasicColor::WHITE_BG:
        return BasicColor::WHITE_BG;
    case OS::BasicColor::RED_BG:
        return BasicColor::RED_BG;
    case OS::BasicColor::GREEN_BG:
        return BasicColor::GREEN_BG;
    case OS::BasicColor::BLUE_BG:
        return BasicColor::BLUE_BG;
    case OS::BasicColor::YELLOW_BG:
        return BasicColor::YELLOW_BG;
    case OS::BasicColor::MAGENTA_BG:
        return BasicColor::MAGENTA_BG;
    case OS::BasicColor::CYAN_BG:
        return BasicColor::CYAN_BG;
    case OS::BasicColor::GREY_BG:
        return BasicColor::GREY_BG;
    case OS::BasicColor::DARK_RED_BG:
        return BasicColor::DARK_RED_BG;
    case OS::BasicColor::DARK_GREEN_BG:
        return BasicColor::DARK_GREEN_BG;
    case OS::BasicColor::DARK_BLUE_BG:
        return BasicColor::DARK_BLUE_BG;
    case OS::BasicColor::DARK_YELLOW_BG:
        return BasicColor::DARK_YELLOW_BG;
    case OS::BasicColor::DARK_MAGENTA_BG:
        return BasicColor::DARK_MAGENTA_BG;
    case OS::BasicColor::DARK_CYAN_BG:
        return BasicColor::DARK_CYAN_BG;
    case OS::BasicColor::DARK_GREY_BG:
        return BasicColor::DARK_GREY_BG;
    default:
        chg::fatal("Unknown color code: {}.", underlying_cast(color));
    }
}

OS::BasicColor colorCast(const BasicColor color)
{
    switch (color) {
    case BasicColor::NONE:
        return OS::BasicColor::NONE;
    case BasicColor::BLACK:
        return OS::BasicColor::BLACK;
    case BasicColor::WHITE:
        return OS::BasicColor::WHITE;
    case BasicColor::RED:
        return OS::BasicColor::RED;
    case BasicColor::GREEN:
        return OS::BasicColor::GREEN;
    case BasicColor::BLUE:
        return OS::BasicColor::BLUE;
    case BasicColor::YELLOW:
        return OS::BasicColor::YELLOW;
    case BasicColor::MAGENTA:
        return OS::BasicColor::MAGENTA;
    case BasicColor::CYAN:
        return OS::BasicColor::CYAN;
    case BasicColor::GREY:
        return OS::BasicColor::GREY;
    case BasicColor::DARK_RED:
        return OS::BasicColor::DARK_RED;
    case BasicColor::DARK_GREEN:
        return OS::BasicColor::DARK_GREEN;
    case BasicColor::DARK_BLUE:
        return OS::BasicColor::DARK_BLUE;
    case BasicColor::DARK_YELLOW:
        return OS::BasicColor::DARK_YELLOW;
    case BasicColor::DARK_MAGENTA:
        return OS::BasicColor::DARK_MAGENTA;
    case BasicColor::DARK_CYAN:
        return OS::BasicColor::DARK_CYAN;
    case BasicColor::DARK_GREY:
        return OS::BasicColor::DARK_GREY;
    // case BasicColor::BLACK_BG:    return OS::BasicColor::BLACK_BG; // commented in original
    case BasicColor::WHITE_BG:
        return OS::BasicColor::WHITE_BG;
    case BasicColor::RED_BG:
        return OS::BasicColor::RED_BG;
    case BasicColor::GREEN_BG:
        return OS::BasicColor::GREEN_BG;
    case BasicColor::BLUE_BG:
        return OS::BasicColor::BLUE_BG;
    case BasicColor::YELLOW_BG:
        return OS::BasicColor::YELLOW_BG;
    case BasicColor::MAGENTA_BG:
        return OS::BasicColor::MAGENTA_BG;
    case BasicColor::CYAN_BG:
        return OS::BasicColor::CYAN_BG;
    case BasicColor::GREY_BG:
        return OS::BasicColor::GREY_BG;
    case BasicColor::DARK_RED_BG:
        return OS::BasicColor::DARK_RED_BG;
    case BasicColor::DARK_GREEN_BG:
        return OS::BasicColor::DARK_GREEN_BG;
    case BasicColor::DARK_BLUE_BG:
        return OS::BasicColor::DARK_BLUE_BG;
    case BasicColor::DARK_YELLOW_BG:
        return OS::BasicColor::DARK_YELLOW_BG;
    case BasicColor::DARK_MAGENTA_BG:
        return OS::BasicColor::DARK_MAGENTA_BG;
    case BasicColor::DARK_CYAN_BG:
        return OS::BasicColor::DARK_CYAN_BG;
    case BasicColor::DARK_GREY_BG:
        return OS::BasicColor::DARK_GREY_BG;
    default:
        chg::fatal("Unknown color code: {}.", underlying_cast(color));
    }
}

HANDLE getStdOutputHandle()
{
    const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE)
        chg::fatal("Failed to retrieve STD_OUTPUT_HANDLE (error code: {}).", GetLastError());
    return handle;
}

OS::BasicColor getCurrentTextColor(const OS::BasicColor onFailure)
{
    static const auto handle = getStdOutputHandle();

    CONSOLE_SCREEN_BUFFER_INFO info;
    if (not GetConsoleScreenBufferInfo(handle, &info))
        return onFailure;  // This fails in GitHub CI.

    const auto winColor = checked_cast<BasicColor>(info.wAttributes);
    return colorCast(winColor);
}

bool tryEnablingVirtualTerminalProcessing()
{
    static const auto handle = getStdOutputHandle();

    DWORD dwMode = 0;
    if (not GetConsoleMode(handle, &dwMode))
        return false;  // This fails in GitHub CI.

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (not SetConsoleMode(handle, dwMode))
        chg::fatal(
            "Failed to set console mode: '{}' (error code: {}).",
            std::bitset<sizeof(dwMode)>(dwMode).to_string('-', '#'),
            GetLastError());

    return true;
}

}  // namespace Windows

#endif

#ifdef CHG_COLOR_PRINT_MODE_ESCSEQ

namespace EscSeq
{

class BasicColor
{
public:
    static inline constexpr std::string_view NONE = "";

    static inline constexpr std::string_view BLACK = "\033[30m";
    static inline constexpr std::string_view WHITE = "\033[97m";

    static inline constexpr std::string_view RED     = "\033[91m";
    static inline constexpr std::string_view GREEN   = "\033[92m";
    static inline constexpr std::string_view BLUE    = "\033[94m";
    static inline constexpr std::string_view YELLOW  = "\033[93m";
    static inline constexpr std::string_view MAGENTA = "\033[95m";
    static inline constexpr std::string_view CYAN    = "\033[96m";
    static inline constexpr std::string_view GREY    = "\033[37m";

    static inline constexpr std::string_view DARK_RED     = "\033[31m";
    static inline constexpr std::string_view DARK_GREEN   = "\033[32m";
    static inline constexpr std::string_view DARK_BLUE    = "\033[34m";
    static inline constexpr std::string_view DARK_YELLOW  = "\033[33m";
    static inline constexpr std::string_view DARK_MAGENTA = "\033[35m";
    static inline constexpr std::string_view DARK_CYAN    = "\033[36m";
    static inline constexpr std::string_view DARK_GREY    = "\033[90m";

    static inline constexpr std::string_view BLACK_BG = "\033[40m";
    static inline constexpr std::string_view WHITE_BG = "\033[107m";

    static inline constexpr std::string_view RED_BG     = "\033[101m";
    static inline constexpr std::string_view GREEN_BG   = "\033[102m";
    static inline constexpr std::string_view BLUE_BG    = "\033[104m";
    static inline constexpr std::string_view YELLOW_BG  = "\033[103m";
    static inline constexpr std::string_view MAGENTA_BG = "\033[105m";
    static inline constexpr std::string_view CYAN_BG    = "\033[106m";
    static inline constexpr std::string_view GREY_BG    = "\033[47m";

    static inline constexpr std::string_view DARK_RED_BG     = "\033[41m";
    static inline constexpr std::string_view DARK_GREEN_BG   = "\033[42m";
    static inline constexpr std::string_view DARK_BLUE_BG    = "\033[44m";
    static inline constexpr std::string_view DARK_YELLOW_BG  = "\033[43m";
    static inline constexpr std::string_view DARK_MAGENTA_BG = "\033[45m";
    static inline constexpr std::string_view DARK_CYAN_BG    = "\033[46m";
    static inline constexpr std::string_view DARK_GREY_BG    = "\033[100m";
};

std::string_view colorCast(const OS::BasicColor color)
{
    switch (color) {
    case OS::BasicColor::NONE:
        return BasicColor::NONE;
    case OS::BasicColor::BLACK:
        return BasicColor::BLACK;
    case OS::BasicColor::WHITE:
        return BasicColor::WHITE;
    case OS::BasicColor::RED:
        return BasicColor::RED;
    case OS::BasicColor::GREEN:
        return BasicColor::GREEN;
    case OS::BasicColor::BLUE:
        return BasicColor::BLUE;
    case OS::BasicColor::YELLOW:
        return BasicColor::YELLOW;
    case OS::BasicColor::MAGENTA:
        return BasicColor::MAGENTA;
    case OS::BasicColor::CYAN:
        return BasicColor::CYAN;
    case OS::BasicColor::GREY:
        return BasicColor::GREY;
    case OS::BasicColor::DARK_RED:
        return BasicColor::DARK_RED;
    case OS::BasicColor::DARK_GREEN:
        return BasicColor::DARK_GREEN;
    case OS::BasicColor::DARK_BLUE:
        return BasicColor::DARK_BLUE;
    case OS::BasicColor::DARK_YELLOW:
        return BasicColor::DARK_YELLOW;
    case OS::BasicColor::DARK_MAGENTA:
        return BasicColor::DARK_MAGENTA;
    case OS::BasicColor::DARK_CYAN:
        return BasicColor::DARK_CYAN;
    case OS::BasicColor::DARK_GREY:
        return BasicColor::DARK_GREY;
    case OS::BasicColor::BLACK_BG:
        return BasicColor::BLACK_BG;
    case OS::BasicColor::WHITE_BG:
        return BasicColor::WHITE_BG;
    case OS::BasicColor::RED_BG:
        return BasicColor::RED_BG;
    case OS::BasicColor::GREEN_BG:
        return BasicColor::GREEN_BG;
    case OS::BasicColor::BLUE_BG:
        return BasicColor::BLUE_BG;
    case OS::BasicColor::YELLOW_BG:
        return BasicColor::YELLOW_BG;
    case OS::BasicColor::MAGENTA_BG:
        return BasicColor::MAGENTA_BG;
    case OS::BasicColor::CYAN_BG:
        return BasicColor::CYAN_BG;
    case OS::BasicColor::GREY_BG:
        return BasicColor::GREY_BG;
    case OS::BasicColor::DARK_RED_BG:
        return BasicColor::DARK_RED_BG;
    case OS::BasicColor::DARK_GREEN_BG:
        return BasicColor::DARK_GREEN_BG;
    case OS::BasicColor::DARK_BLUE_BG:
        return BasicColor::DARK_BLUE_BG;
    case OS::BasicColor::DARK_YELLOW_BG:
        return BasicColor::DARK_YELLOW_BG;
    case OS::BasicColor::DARK_MAGENTA_BG:
        return BasicColor::DARK_MAGENTA_BG;
    case OS::BasicColor::DARK_CYAN_BG:
        return BasicColor::DARK_CYAN_BG;
    case OS::BasicColor::DARK_GREY_BG:
        return BasicColor::DARK_GREY_BG;
    default:
        chg::fatal("Unknown color code: {}.", underlying_cast(color));
    }
}

}  // namespace EscSeq

#endif

OS::BasicColor OS::brighten(const BasicColor color)
{
    switch (color) {
    case BasicColor::DARK_RED:
        return BasicColor::RED;
    case BasicColor::DARK_GREEN:
        return BasicColor::GREEN;
    case BasicColor::DARK_BLUE:
        return BasicColor::BLUE;
    case BasicColor::DARK_YELLOW:
        return BasicColor::YELLOW;
    case BasicColor::DARK_MAGENTA:
        return BasicColor::MAGENTA;
    case BasicColor::DARK_CYAN:
        return BasicColor::CYAN;
    case BasicColor::DARK_GREY:
        return BasicColor::GREY;
    case BasicColor::DARK_RED_BG:
        return BasicColor::RED_BG;
    case BasicColor::DARK_GREEN_BG:
        return BasicColor::GREEN_BG;
    case BasicColor::DARK_BLUE_BG:
        return BasicColor::BLUE_BG;
    case BasicColor::DARK_YELLOW_BG:
        return BasicColor::YELLOW_BG;
    case BasicColor::DARK_MAGENTA_BG:
        return BasicColor::MAGENTA_BG;
    case BasicColor::DARK_CYAN_BG:
        return BasicColor::CYAN_BG;
    case BasicColor::DARK_GREY_BG:
        return BasicColor::GREY_BG;
    case BasicColor::NONE:
    case BasicColor::BLACK:
    case BasicColor::WHITE:
    case BasicColor::RED:
    case BasicColor::GREEN:
    case BasicColor::BLUE:
    case BasicColor::YELLOW:
    case BasicColor::MAGENTA:
    case BasicColor::CYAN:
    case BasicColor::GREY:
    case BasicColor::BLACK_BG:
    case BasicColor::WHITE_BG:
    case BasicColor::RED_BG:
    case BasicColor::GREEN_BG:
    case BasicColor::BLUE_BG:
    case BasicColor::YELLOW_BG:
    case BasicColor::MAGENTA_BG:
    case BasicColor::CYAN_BG:
    case BasicColor::GREY_BG:
        return color;
    default:
        chg::fatal("Unknown color code: {}.", underlying_cast(color));
    }
}

OS::BasicColor OS::darken(const BasicColor color)
{
    switch (color) {
    case BasicColor::RED:
        return BasicColor::DARK_RED;
    case BasicColor::GREEN:
        return BasicColor::DARK_GREEN;
    case BasicColor::BLUE:
        return BasicColor::DARK_BLUE;
    case BasicColor::YELLOW:
        return BasicColor::DARK_YELLOW;
    case BasicColor::MAGENTA:
        return BasicColor::DARK_MAGENTA;
    case BasicColor::CYAN:
        return BasicColor::DARK_CYAN;
    case BasicColor::GREY:
        return BasicColor::DARK_GREY;

    case BasicColor::RED_BG:
        return BasicColor::DARK_RED_BG;
    case BasicColor::GREEN_BG:
        return BasicColor::DARK_GREEN_BG;
    case BasicColor::BLUE_BG:
        return BasicColor::DARK_BLUE_BG;
    case BasicColor::YELLOW_BG:
        return BasicColor::DARK_YELLOW_BG;
    case BasicColor::MAGENTA_BG:
        return BasicColor::DARK_MAGENTA_BG;
    case BasicColor::CYAN_BG:
        return BasicColor::DARK_CYAN_BG;
    case BasicColor::GREY_BG:
        return BasicColor::DARK_GREY_BG;
    case BasicColor::NONE:
    case BasicColor::BLACK:
    case BasicColor::WHITE:
    case BasicColor::DARK_RED:
    case BasicColor::DARK_GREEN:
    case BasicColor::DARK_BLUE:
    case BasicColor::DARK_YELLOW:
    case BasicColor::DARK_MAGENTA:
    case BasicColor::DARK_CYAN:
    case BasicColor::DARK_GREY:
    case BasicColor::BLACK_BG:
    case BasicColor::WHITE_BG:
    case BasicColor::DARK_RED_BG:
    case BasicColor::DARK_GREEN_BG:
    case BasicColor::DARK_BLUE_BG:
    case BasicColor::DARK_YELLOW_BG:
    case BasicColor::DARK_MAGENTA_BG:
    case BasicColor::DARK_CYAN_BG:
    case BasicColor::DARK_GREY_BG:
        return color;
    default:
        chg::fatal("Unknown color code: {}.", underlying_cast(color));
    }
}

namespace
{

OS::BasicColor& currentTextColor()
{
#if defined(CHG_BUILD_WINDOWS)
    static auto currentTextColor = Windows::getCurrentTextColor(OS::BasicColor::WHITE);
#else
    // Can't obtain the initial color on Linux, so we assume WHITE.
    static auto currentTextColor = OS::BasicColor::WHITE;
#endif

    return currentTextColor;
}

}  // namespace

OS::BasicColor OS::getTextColor() { return currentTextColor(); }

void OS::setTextColor(const BasicColor color, std::ostream& os)
{
    // Changing the text color is expected to always be followed by a text output procedure.
    // A locking mechanism is needed at the call site to ensure the text has the desired color, not
    // here.
    CHG_NEVER_CONCURRENT();

#if defined(CHG_COLOR_PRINT_MODE_ESCSEQ)
    if (color == currentTextColor() || color == OS::BasicColor::NONE)
        return;
    if (&os != &std::cout && &os != &std::cerr && &os != &std::clog)
        return;

    #if defined(CHG_BUILD_WINDOWS)
    // Needed in order to recognize escape sequences.
    static const auto _ = Windows::tryEnablingVirtualTerminalProcessing();
    #endif

    currentTextColor() = color;
    const auto escSeq  = EscSeq::colorCast(color);
    os << escSeq;

#elif defined(CHG_COLOR_PRINT_MODE_WINAPI)
    if (color == CurrentTextColor || color == OS::BasicColor::NONE)
        return;
    if (&os != &std::cout && &os != &std::cerr && &os != &std::clog)
        return;

    static const auto handle = Windows::getStdOutputHandle();

    CurrentTextColor = color;

    const auto winColor = underlying_cast(Windows::colorCast(color));
    if (not SetConsoleTextAttribute(handle, winColor))
        chg::fatal(
            "Failed to to set console text attribute: '{}' (error code: {}).",
            std::bitset<sizeof(winColor)>(winColor).to_string('-', '#'),
            GetLastError());

#elif defined(CHG_COLOR_PRINT_MODE_DISABLED)
#else
    #error No CHG_COLOR_PRINT_MODE defined.
#endif
}
