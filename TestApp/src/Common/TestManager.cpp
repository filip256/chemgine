#include "Common/TestManager.hpp"
#include "Unit/EstimatorUnitTests.hpp"
#include "Unit/StructureUnitTests.hpp"
#include "Unit/DefUnitTests.hpp"
#include "Unit/ModuleUnitTest.hpp"
#include "Unit/MixtureUnitTests.hpp"
#include "Performance/DefPerfTests.hpp"
#include "Performance/StructurePerfTests.hpp"
#include "Performance/EstimatorPerfTests.hpp"
#include "Performance/PerformanceReport.hpp"
#include "PathUtils.hpp"
#include "BuildUtils.hpp"

UnitTests::UnitTests(const std::regex& filter) noexcept :
	UnitTestGroup("Unit", filter)
{
	registerTest<EstimatorUnitTests>("Estimator");
	registerTest<StructureUnitTests>("Structure", "./TestFiles/builtin/radicals.cdef");
	registerTest<DefUnitTests>("Def", "./TestFiles/builtin/radicals.cdef");
	registerTest<ModuleUnitTest>("Module", "./TestFiles/builtin.cdef");
	registerTest<MixtureUnitTests>("Mixture", "./TestFiles/builtin.cdef");
}


PerfTests::PerfTests(const std::regex& filter) noexcept :
	PerfTestGroup("Perf", filter),
	timingUnitTests("Timing", filter)
{
	registerTest<EstimatorPerfTests>("Estimator");
	registerTest<StructurePerfTests>("Structure", "./TestFiles/builtin/radicals.cdef");
	registerTest<DefPerfTests>("Def", "./TestFiles/builtin/radicals.cdef");
}

std::chrono::nanoseconds PerfTests::run(PerformanceReport& report)
{
	timingUnitTests.run();
	return PerfTestGroup::run(report);
}


TestManager::TestManager(std::regex&& filter) noexcept :
	filter(std::move(filter)),
	unitTests(this->filter),
	perfTests(this->filter)
{}

void TestManager::runUnit()
{
	if (unitTests.getTestCount() == 0)
	{
		Log(this).info("No unit tests match the given filter.");
		return;
	}

	if (not unitTests.run())
		Log(this).error("Some unit tests failed.");
	else
		Log(this).success("All unit tests passed.");
}

void TestManager::runPerf()
{
	if (perfTests.getTestCount() == 0)
	{
		Log(this).info("No perf tests match the given filter.");
		return;
	}

	const auto current = perfTests.generateReport();

	PerformanceReport prev;
	const auto& buildName = Utils::getBuildTypeName();

	if (prev.load("./Reports/perf_" + buildName + "_LTS.txt"))
		Log(this).info("Performance report:\n{0}", prev.compare(current).toString());
	else
	{
		Utils::createDir("./Reports");
		Log(this).warn("Missing previous performance report.");
	}

	current.dump("./Reports/perf_" + buildName + "_LTS.txt");
}
