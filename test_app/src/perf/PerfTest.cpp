#include "perf/PerfTest.hpp"

#include "io/Log.hpp"
#include "perf/PerformanceReport.hpp"
#include "utils/Casts.hpp"
#include "utils/STL.hpp"

PerfTest::PerfTest(std::string&& name) noexcept :
    name(std::move(name))
{}

const std::string& PerfTest::getName() const { return name; }

bool PerfTest::isSkipped(const std::regex& filter) const { return not std::regex_match(name, filter); }

std::chrono::nanoseconds TimedTest::WarmUpTime = std::chrono::seconds(2);

TimedTest::TimedTest(std::string&& name, const std::variant<size_t, std::chrono::nanoseconds> limit) noexcept :
    PerfTest(std::move(name)),
    limit(limit)
{}

void TimedTest::setup() {}

void TimedTest::preTask() {}

void TimedTest::postTask() {}

void TimedTest::cleanup() {}

size_t TimedTest::getTestCount() const { return 1; }

std::chrono::nanoseconds TimedTest::getEstimatedRunTime() const
{
    const auto eta = std::holds_alternative<size_t>(limit) ? std::chrono::nanoseconds(std::get<size_t>(limit) * 1'000)
                                                           : std::get<std::chrono::nanoseconds>(limit);

    return eta + WarmUpTime + std::chrono::milliseconds(100);
}

void TimedTest::runWarmUp()
{
    auto minTime = WarmUpTime;
    while (minTime.count() > 0) {
        const auto start = std::chrono::high_resolution_clock::now();
        preTask();
        task();
        postTask();
        const auto time = std::chrono::high_resolution_clock::now() - start;

        minTime -= std::max(time, std::chrono::nanoseconds(1));
    }
}

OS::ExecutionConfig TimedTest::stabilizeExecution()
{
    static const auto processorMask           = OS::getAvailablePhysicalProcessorMask();
    const auto        normalProcessorAffinity = OS::setCurrentThreadProcessorAffinity(processorMask);

    const auto normalPriority = OS::getCurrentProcessPriority();
    OS::setCurrentProcessPriority(OS::ProcessPriority::REALTIME_PRIORITY_CLASS);

    return {normalProcessorAffinity, normalPriority};
}

void TimedTest::restoreExecution(const OS::ExecutionConfig normalProperties)
{
    OS::setCurrentProcessPriority(normalProperties.processPriority);
    OS::setCurrentThreadProcessorAffinity(normalProperties.processorAffinityMask);
}

TimingResult TimedTest::runCounted(const size_t repetitions)
{
    auto                  totalTime = std::chrono::nanoseconds(0);
    std::vector<uint32_t> timeHistory;
    timeHistory.reserve(repetitions);

    setup();
    runWarmUp();

    for (size_t i = 0; i < repetitions; ++i) {
        preTask();
        const auto start = std::chrono::high_resolution_clock::now();
        task();
        const auto time = std::chrono::high_resolution_clock::now() - start;
        postTask();

        totalTime += time;
        timeHistory.emplace_back(checked_cast<uint32_t>(time.count()));
    }

    cleanup();

    const auto avgTime    = totalTime / repetitions;
    const auto medianTime = std::chrono::nanoseconds(utils::getAveragedMedian(timeHistory));

    return {avgTime, medianTime};
}

TimingResult TimedTest::runTimed(std::chrono::nanoseconds minTime)
{
    auto                  totalTime = std::chrono::nanoseconds(0);
    std::vector<uint32_t> timeHistory;

    setup();
    runWarmUp();

    while (minTime.count() > 0) {
        preTask();
        const auto start = std::chrono::high_resolution_clock::now();
        task();
        const auto time = std::chrono::high_resolution_clock::now() - start;
        postTask();

        totalTime += time;
        timeHistory.emplace_back(checked_cast<uint32_t>(time.count()));
        minTime -= std::max(time, std::chrono::nanoseconds(1));
    }

    cleanup();

    const auto avgTime    = totalTime / timeHistory.size();
    const auto medianTime = std::chrono::nanoseconds(utils::getAveragedMedian(timeHistory));

    return {avgTime, medianTime};
}

TimingResult TimedTest::run(PerformanceReport& report)
{
    const auto normalConfig = stabilizeExecution();
    LogBase::hide();

    const auto time = std::holds_alternative<size_t>(limit) ? runCounted(std::get<size_t>(limit))
                                                            : runTimed(std::get<std::chrono::nanoseconds>(limit));

    LogBase::unhide();
    restoreExecution(normalConfig);

    report.add(getName(), time);

    return time;
}

bool TimedTest::isSkipped(const std::regex& filter) const
{
    bool isInactive = std::holds_alternative<size_t>(limit) ? std::get<size_t>(limit) == 0
                                                            : std::get<std::chrono::nanoseconds>(limit).count() <= 0;

    return isInactive || PerfTest::isSkipped(filter);
}

PerfTestGroup::PerfTestGroup(std::string&& name, const std::regex& filter) noexcept :
    PerfTest(std::move(name)),
    filter(filter)
{}

size_t PerfTestGroup::getTestCount() const { return testCount; }

std::chrono::nanoseconds PerfTestGroup::getEstimatedRunTime() const { return estimatedRunTime; }

TimingResult PerfTestGroup::run(PerformanceReport& report)
{
    TimingResult totalTime(std::chrono::nanoseconds(0), std::chrono::nanoseconds(0));

    Log(this).info(
        "{0}: Running {1} sub-tests... (ETA: {2})",
        getName(),
        testCount,
        utils::formatTime(estimatedRunTime, utils::TimeFormat::HUMAN_HH_MM_SS));
    LogBase::nest();

    for (size_t i = 0; i < tests.size(); ++i) {
        // Setups
        if (tests[i]->getTestCount() == 0) {
            tests[i]->run(report);
            continue;
        }

        Log(this).info("\rRunning {0}...", tests[i]->getName());

        LogBase::nest();
        const auto time = tests[i]->run(report);
        LogBase::unnest();

        totalTime += time;

        const auto avgTimeInMs    = time.averageTime.count() / 1'000'000.0;
        const auto medianTimeInMs = time.medianTime.count() / 1'000'000.0;

        Log(this).info(
            "\rTest {0} took {1}ms (avg) / {2}ms (med).",
            tests[i]->getName(),
            std::format("{:.4f}", avgTimeInMs),
            std::format("{:.4f}", medianTimeInMs));
        Log(this).info("\0");
    }

    LogBase::unnest();

    report.add(getName(), totalTime);
    return totalTime;
}

PerformanceReport PerfTestGroup::generateReport()
{
    PerformanceReport report;
    run(report);
    report.setTimestamp();

    return report;
}

bool PerfTestGroup::isSkipped(const std::regex&) const
{
    return testCount == 0;  // Skip empty groups
}
