#pragma once

#include "Unit/UnitTest.hpp"
#include "Performance/PerfTest.hpp"
#include "Unit/Tests/TimingUnitTests.hpp"

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
	UnitTests unitTests;
	PerfTests perfTests;

public:
	TestManager(std::regex&& filter) noexcept;

	void runUnit();
	void runPerf();
};
