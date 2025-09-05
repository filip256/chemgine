#pragma once

#include <iostream>

class Input
{
public:
    static std::istream& inputStream;
    static std::ostream& outputStream;

    static std::string get(const std::string& msg = "");
};
