#include "TimingUnitTests.hpp"
#include "PerformanceReport.hpp"
#include "Log.hpp"

#include <thread>

TimingUnitTest::DummyPerfTest::DummyPerfTest(
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	const std::chrono::nanoseconds waitTime
) noexcept :
	TimedTest("dummy", limit),
	waitTime(waitTime)
{}

void TimingUnitTest::DummyPerfTest::task()
{
	const auto start = std::chrono::high_resolution_clock::now();
	while (std::chrono::high_resolution_clock::now() - start < waitTime) {}
}

std::chrono::nanoseconds TimingUnitTest::DummyPerfTest::getWaitTime() const
{
	return waitTime;
}


TimingUnitTest::TimingUnitTest(
	const std::string& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	const std::chrono::nanoseconds waitTime,
	const float_h threshold
) noexcept :
	UnitTest(name + '_' + (std::holds_alternative<uint64_t>(limit) ?
		std::to_string(std::get<uint64_t>(limit)) :
		std::to_string(std::get<std::chrono::nanoseconds>(limit).count()) + "ns") +
		'_' + std::to_string(waitTime.count()) + "ns"),
	perfTest(limit, waitTime),
	threshold(threshold)
{}

bool TimingUnitTest::run()
{
	PerformanceReport report;
	const auto act = perfTest.run(report).count();
	const auto ref = perfTest.getWaitTime().count();

	const auto error = std::abs((static_cast<float_h>(act - ref) / ref) * 100.0);
	if (error > threshold)
	{
		Log(this).error("Error: {0}% (act= {1}ns, ref= {2}ns) exceeded the test threshold: {3}%.",
			Linguistics::formatFloatingPoint(error), act, ref, Linguistics::formatFloatingPoint(threshold));
		return false;
	}

	return true;
}


TimingUnitTests::TimingUnitTests(
	std::string&& name,
	const std::regex& filter
) noexcept :
	UnitTestGroup(std::move(name), filter)
{
	registerTest<TimingUnitTest>("counted", uint64_t(20), std::chrono::milliseconds(10), 0.4);
	registerTest<TimingUnitTest>("counted", uint64_t(100), std::chrono::milliseconds(10), 0.2);

	registerTest<TimingUnitTest>("counted", uint64_t(30), std::chrono::microseconds(100), 6.0);
	registerTest<TimingUnitTest>("counted", uint64_t(20), std::chrono::milliseconds(1), 4.0);
	registerTest<TimingUnitTest>("counted", uint64_t(20), std::chrono::milliseconds(10), 0.2);
	registerTest<TimingUnitTest>("counted", uint64_t(10), std::chrono::milliseconds(100), 0.08);
	registerTest<TimingUnitTest>("counted", uint64_t(2), std::chrono::seconds(1), 0.004);

	registerTest<TimingUnitTest>("timed", std::chrono::milliseconds(200), std::chrono::milliseconds(10), 0.4);
	registerTest<TimingUnitTest>("timed", std::chrono::seconds(1), std::chrono::milliseconds(10), 0.2);
}
