#pragma once

#include <string>
#include <vector>

namespace utils
{

bool isWhiteSpace(const char c);

void strip(std::string& str, bool (*pred)(const char) = [](const auto c) -> bool { return isWhiteSpace(c); });

std::string strip(const std::string& str, bool (*pred)(const char) = [](const auto c) -> bool { return isWhiteSpace(c); });

std::vector<std::string> split(const std::string& line, const char separator, const bool ignoreEmpty = false);

std::vector<std::string> split(
    const std::string& line,
    const char         separator,
    const char         ignoreSectionBegin,
    const char         ignoreSectionEnd,
    const bool         ignoreEmpty = false);

std::vector<std::string> split(
    const std::string& line,
    const char         separator,
    const std::string& ignoreSectionBegins,
    const std::string& ignoreSectionEnds,
    const bool         ignoreEmpty = false);

std::vector<std::vector<std::string>>
splitLists(const std::string& line, const char outerSeparator, const char innerSeparator, const bool ignoreEmpty = false);

};  // namespace utils
