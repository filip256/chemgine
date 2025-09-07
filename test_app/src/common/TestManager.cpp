#include "common/TestManager.hpp"

#include "perf/PerformanceReport.hpp"
#include "perf/tests/DefPerfTests.hpp"
#include "perf/tests/EstimatorPerfTests.hpp"
#include "perf/tests/FPSPerfTests.hpp"
#include "perf/tests/StructurePerfTests.hpp"
#include "unit/tests/DefUnitTests.hpp"
#include "unit/tests/EstimatorUnitTests.hpp"
#include "unit/tests/MixtureUnitTests.hpp"
#include "unit/tests/ModuleUnitTests.hpp"
#include "unit/tests/ProcessUnitTests.hpp"
#include "unit/tests/StructureUnitTests.hpp"
#include "unit/tests/UtilsUnitTests.hpp"

UnitTests::UnitTests(const std::regex& filter) noexcept :
    UnitTestGroup("Unit", filter)
{
    registerTest<UtilsUnitTests>("utils");
    registerTest<EstimatorUnitTests>("Estimator");
    registerTest<StructureUnitTests>("Structure", "./data/builtin/radicals.cdef");
    registerTest<DefUnitTests>("def", "./data/builtin/radicals.cdef");
    registerTest<ModuleUnitTest>("Module", "./data/builtin.cdef");
    registerTest<MixtureUnitTests>("Mixture", "./data/builtin.cdef");
    registerTest<ProcessUnitTests>("Process");
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

bool TestManager::runUnit()
{
    const auto testCount = unitTests.getTestCount();
    if (testCount == 0) {
        Log(this).warn("No unit tests match the given filter.");
        return true;
    }
    Log(this).info("{} unit tests match the given filter.", testCount);

    if (not unitTests.run()) {
        Log(this).error("Some unit tests failed.");
        return false;
    }

    Log(this).success("All unit tests passed.");
    return true;
}

PerformanceReport TestManager::runPerf()
{
    if (perfTests.getTestCount() == 0) {
        Log(this).info("No perf tests match the given filter.");
    }
    return perfTests.generateReport();
}
