#include "utils/Bin.hpp"

#include <sstream>
#include <iomanip>

std::string utils::toBin(const std::string& str, const char byteDelim, const char octetDelim)
{
    if (str.empty())
        return "";

    std::ostringstream os;

    const auto printChar = [&os](const uint8_t c)
        {
            for (uint8_t bit = 7; bit-- > 0;)
                os << ((c >> bit) & 1);
        };

    for (size_t i = 0; i < str.size() - 1; ++i)
    {
        printChar(str[i]);
        os << (octetDelim != '\0' && i % 8 == 7 ? octetDelim : byteDelim);
    }
    printChar(str.back());

    return os.str();
}

std::string utils::toHex(const std::string& str, const char byteDelim, const char octetDelim)
{
    if (str.empty())
        return "";

    std::ostringstream os;
    os << std::hex << std::setfill('0');

    for (size_t i = 0; i < str.size() - 1; ++i)
    {
        os << std::setw(2) << static_cast<uint16_t>(str[i]);
        os << (octetDelim != '\0' && i % 8 == 7 ? octetDelim : byteDelim);
    }
    os << std::setw(2) << static_cast<uint16_t>(str.back());

    return os.str();
}
