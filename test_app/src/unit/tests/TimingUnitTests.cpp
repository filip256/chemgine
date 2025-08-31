#include "unit/tests/TimingUnitTests.hpp"

#include "io/Log.hpp"
#include "perf/PerformanceReport.hpp"

#include <thread>

TimingUnitTest::DummyPerfTest::DummyPerfTest(
    const std::variant<size_t, std::chrono::nanoseconds> limit, const std::chrono::nanoseconds waitTime) noexcept :
    TimedTest("dummy", limit),
    waitTime(waitTime)
{}

void TimingUnitTest::DummyPerfTest::task()
{
    const auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start < waitTime) {}
}

std::chrono::nanoseconds TimingUnitTest::DummyPerfTest::getWaitTime() const { return waitTime; }

TimingUnitTest::TimingUnitTest(
    const std::string&                                   name,
    const std::variant<size_t, std::chrono::nanoseconds> limit,
    const std::chrono::nanoseconds                       waitTime,
    const float_h                                        threshold) noexcept :
    UnitTest(
        name +
        '_' +
        (std::holds_alternative<size_t>(limit)
             ? std::to_string(std::get<size_t>(limit))
             : std::to_string(std::get<std::chrono::nanoseconds>(limit).count()) + "ns") +
        '_' +
        std::to_string(waitTime.count()) +
        "ns"),
    perfTest(limit, waitTime),
    threshold(threshold)
{}

bool TimingUnitTest::run()
{
    PerformanceReport report;
    const auto        act = perfTest.run(report);
    const auto        ref = perfTest.getWaitTime().count();

    const auto minAbsDiff = std::min(std::abs(act.averageTime.count() - ref), std::abs(act.medianTime.count() - ref));

    const auto error = (static_cast<float_h>(minAbsDiff) / ref) * 100.0;
    if (error > threshold) {
        Log(this).error(
            "Error: {0}% exceeded the test threshold: {1}%.",
            std::format("{:f}", error),
            std::format("{:f}", threshold));
        return false;
    }

    return true;
}

TimingUnitTests::TimingUnitTests(std::string&& name, const std::regex& filter) noexcept :
    UnitTestGroup(std::move(name), filter)
{
    registerTest<TimingUnitTest>("counted", size_t(20), std::chrono::milliseconds(10), 0.3);
    registerTest<TimingUnitTest>("counted", size_t(100), std::chrono::milliseconds(10), 0.2);

    registerTest<TimingUnitTest>("counted", size_t(30), std::chrono::microseconds(100), 5.0);
    registerTest<TimingUnitTest>("counted", size_t(20), std::chrono::milliseconds(1), 3.0);
    registerTest<TimingUnitTest>("counted", size_t(20), std::chrono::milliseconds(10), 0.1);
    registerTest<TimingUnitTest>("counted", size_t(10), std::chrono::milliseconds(100), 0.06);

    registerTest<TimingUnitTest>("timed", std::chrono::milliseconds(200), std::chrono::milliseconds(10), 0.3);
    registerTest<TimingUnitTest>("timed", std::chrono::seconds(1), std::chrono::milliseconds(10), 0.2);
}
