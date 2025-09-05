#pragma once

#include "perf/PerfTest.hpp"
#include "unit/UnitTest.hpp"
#include "unit/tests/TimingUnitTests.hpp"

class UnitTests : public UnitTestGroup
{
public:
    UnitTests(const std::regex& filter) noexcept;
};

class PerfTests : public PerfTestGroup
{
private:
    TimingUnitTests timingUnitTests;

public:
    PerfTests(const std::regex& filter) noexcept;

    TimingResult run(PerformanceReport& report) override final;
};

class TestManager
{
private:
    std::regex filter;
    UnitTests  unitTests;
    PerfTests  perfTests;

public:
    TestManager(std::regex&& filter) noexcept;

    bool              runUnit();
    PerformanceReport runPerf();
};
