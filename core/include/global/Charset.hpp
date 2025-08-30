#pragma once

namespace ASCII
{

#if defined(CHG_EXTENDED_CHAR_SET_CP437)
constexpr char MiddleDot1 = static_cast<char>(0xF9);
constexpr char MiddleDot2 = static_cast<char>(0xFA);

constexpr char BlockFade1 = static_cast<char>(0xB0);
constexpr char BlockFade2 = static_cast<char>(0xB1);
constexpr char BlockFade3 = static_cast<char>(0xB2);
constexpr char BlockFull  = static_cast<char>(0xDB);

constexpr char LineH = static_cast<char>(0xC4);
constexpr char LineV = static_cast<char>(0xB3);

constexpr char DoubleLineH = static_cast<char>(0xCD);
constexpr char DoubleLineV = static_cast<char>(0xBA);

constexpr char TripleLineH = static_cast<char>(0xF0);

constexpr char CornerTopLeft     = static_cast<char>(0xDA);
constexpr char CornerTopRight    = static_cast<char>(0xBF);
constexpr char CornerBottomLeft  = static_cast<char>(0xC0);
constexpr char CornerBottomRight = static_cast<char>(0xD9);

constexpr char DoubleCornerTopLeft     = static_cast<char>(0xC9);
constexpr char DoubleCornerTopRight    = static_cast<char>(0xBB);
constexpr char DoubleCornerBottomLeft  = static_cast<char>(0xC8);
constexpr char DoubleCornerBottomRight = static_cast<char>(0xBC);

constexpr char JunctionUp    = static_cast<char>(0xC1);
constexpr char JunctionDown  = static_cast<char>(0xC2);
constexpr char JunctionRight = static_cast<char>(0xC3);
constexpr char JunctionLeft  = static_cast<char>(0xB4);
constexpr char Cross         = static_cast<char>(0xC5);

constexpr char AshUppercase = static_cast<char>(0x92);
constexpr char Epsilon      = static_cast<char>(0xEE);

#elif defined(CHG_EXTENDED_CHAR_SET_ASCII)
constexpr char MiddleDot1 = '.';
constexpr char MiddleDot2 = '.';

constexpr char BlockFade1 = ' ';
constexpr char BlockFade2 = ' ';
constexpr char BlockFade3 = ' ';
constexpr char BlockFull  = '#';

constexpr char LineH = '-';
constexpr char LineV = '|';

constexpr char DoubleLineH = '=';
constexpr char DoubleLineV = '=';

constexpr char TripleLineH = '#';

constexpr char CornerTopLeft     = '+';
constexpr char CornerTopRight    = '+';
constexpr char CornerBottomLeft  = '+';
constexpr char CornerBottomRight = '+';

constexpr char DoubleCornerTopLeft     = '+';
constexpr char DoubleCornerTopRight    = '+';
constexpr char DoubleCornerBottomLeft  = '+';
constexpr char DoubleCornerBottomRight = '+';

constexpr char JunctionUp    = '+';
constexpr char JunctionDown  = '+';
constexpr char JunctionRight = '+';
constexpr char JunctionLeft  = '+';
constexpr char Cross         = '+';

constexpr char AshUppercase = 'E';
constexpr char Epsilon      = '$';

#else
    #error No CHG_EXTENDED_CHAR_SET defined.
#endif

}  // namespace ASCII
