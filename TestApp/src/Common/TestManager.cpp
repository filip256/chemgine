#include "Common/TestManager.hpp"
#include "Unit/Tests/EstimatorUnitTests.hpp"
#include "Unit/Tests/StructureUnitTests.hpp"
#include "Unit/Tests/DefUnitTests.hpp"
#include "Unit/Tests/ModuleUnitTest.hpp"
#include "Unit/Tests/MixtureUnitTests.hpp"
#include "Performance/Tests/DefPerfTests.hpp"
#include "Performance/Tests/FPSPerfTests.hpp"
#include "Performance/Tests/StructurePerfTests.hpp"
#include "Performance/Tests/EstimatorPerfTests.hpp"
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
	registerTest<FPSPerfTests>("FPS", "./TestFiles/builtin.cdef");
}

TimingResult PerfTests::run(PerformanceReport& report)
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
		Log(this).info("Performance report:\n{0}", CHG_DELAYED_EVAL(current.compare(prev).toString()));
	else
	{
		Utils::createDir("./Reports");
		Log(this).warn("Missing previous performance report.");
	}

	current.dump("./Reports/perf_" + buildName + "_LTS.txt");
}
