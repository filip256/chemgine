#include "common/TestManager.hpp"
#include "unit/tests/EstimatorUnitTests.hpp"
#include "unit/tests/StructureUnitTests.hpp"
#include "unit/tests/DefUnitTests.hpp"
#include "unit/tests/ModuleUnitTest.hpp"
#include "unit/tests/MixtureUnitTests.hpp"
#include "perf/tests/DefPerfTests.hpp"
#include "perf/tests/FPSPerfTests.hpp"
#include "perf/tests/StructurePerfTests.hpp"
#include "perf/tests/EstimatorPerfTests.hpp"
#include "perf/PerformanceReport.hpp"
#include "utils/Path.hpp"
#include "utils/Build.hpp"

UnitTests::UnitTests(const std::regex& filter) noexcept :
	UnitTestGroup("Unit", filter)
{
	registerTest<EstimatorUnitTests>("Estimator");
	registerTest<StructureUnitTests>("Structure", "./data/builtin/radicals.cdef");
	registerTest<DefUnitTests>("def", "./data/builtin/radicals.cdef");
	registerTest<ModuleUnitTest>("Module", "./data/builtin.cdef");
	registerTest<MixtureUnitTests>("Mixture", "./data/builtin.cdef");
}


PerfTests::PerfTests(const std::regex& filter) noexcept :
	PerfTestGroup("Perf", filter),
	timingUnitTests("Timing", filter)
{
	registerTest<EstimatorPerfTests>("Estimator");
	registerTest<StructurePerfTests>("Structure", "./data/builtin/radicals.cdef");
	registerTest<DefPerfTests>("def", "./data/builtin/radicals.cdef");
	registerTest<FPSPerfTests>("FPS", "./data/builtin.cdef");
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
	const auto testCount = unitTests.getTestCount();
	if (testCount == 0)
	{
		Log(this).warn("No unit tests match the given filter.");
		return;
	}
	Log(this).info("{} unit tests match the given filter.", testCount);

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
	const auto& buildName = utils::getBuildTypeName();

	if (prev.load("./reports/perf_" + buildName + "_LTS.txt"))
		Log(this).info("Performance report:\n{0}", CHG_DELAYED_EVAL(current.compare(prev).toString()));
	else
	{
		utils::createDir("./reports");
		Log(this).warn("Missing previous performance report.");
	}

	current.dump("./reports/perf_" + buildName + "_LTS.txt");
}
