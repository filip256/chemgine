#pragma once

#include "EstimatorUnitTests.hpp"
#include "StructureUnitTests.hpp"
#include "DefUnitTests.hpp"
#include "ModuleUnitTest.hpp"
#include "MixtureUnitTests.hpp"
#include "TimingUnitTests.hpp"
#include "DefPerfTests.hpp"
#include "StructurePerfTests.hpp"
#include "EstimatorPerfTests.hpp"
#include "PerformanceReport.hpp"
#include "BuildUtils.hpp"

class UnitTests : public UnitTestGroup
{
public:
	UnitTests(const std::regex& filter) noexcept :
		UnitTestGroup("Unit", filter)
	{
		registerTest<EstimatorUnitTests>("Estimator");
		registerTest<StructureUnitTests>("Structure", "./Data/builtin/radicals.cdef");
		registerTest<DefUnitTests>("Def", "./Data/builtin/radicals.cdef");
		registerTest<ModuleUnitTest>("Module", "./Data/builtin.cdef");
		registerTest<MixtureUnitTests>("Mixture", "./Data/builtin.cdef");
	}
};

class PerfTests : public PerfTestGroup
{
private:
	TimingUnitTests timingUnitTests;

public:
	PerfTests(const std::regex& filter) noexcept :
		PerfTestGroup("Perf", filter),
		timingUnitTests("Timing", filter)
	{
		registerTest<EstimatorPerfTests>("Estimator");
		registerTest<StructurePerfTests>("Structure", "./Data/builtin/radicals.cdef");
		registerTest<DefPerfTests>("Def");
	}

	std::chrono::nanoseconds run(PerformanceReport& report) override
	{
		timingUnitTests.run();
		return PerfTestGroup::run(report);
	}
};

class TestManager
{
private:
	std::regex filter;
	UnitTests unitTests;
	PerfTests perfTests;

public:
	TestManager(const std::string& filter = ".*") noexcept :
		filter(std::regex(filter, std::regex::optimize)),
		unitTests(this->filter),
		perfTests(this->filter)
	{}

	void runUnit()
	{
		if (unitTests.getTestCount() == 0)
		{
			Log(this).info("No unit tests match the given filter.");
			return;
		}

		unitTests.run();
	}

	void runPerf()
	{
		if (perfTests.getTestCount() == 0)
		{
			Log(this).info("No perf tests match the given filter.");
			return;
		}

		const auto& buildName = Utils::getBuildTypeName();
		const auto current = perfTests.generateReport();

		PerformanceReport prev;
		if(prev.load("./Out/Reports/perf_" + buildName + "_LTS.txt"))
			Log(this).info("Performance report:\n{0}", prev.compare(current).toString());
		else
			Log(this).warn("Missing previous performance report");

		current.dump("./Out/Reports/perf_" + buildName + "_LTS.txt");
	}
};
