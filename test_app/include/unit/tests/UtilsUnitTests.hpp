#pragma once

#include "unit/UnitTest.hpp"

class UtilsUnitTests : public UnitTestGroup
{
public:
    UtilsUnitTests(std::string&& name, const std::regex& filter) noexcept;
};
