#pragma once

#include "unit/UnitTest.hpp"

class ProcessUnitTests : public UnitTestGroup
{
public:
    ProcessUnitTests(std::string&& name, const std::regex& filter) noexcept;
};
