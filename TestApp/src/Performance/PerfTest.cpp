#include "Performance/PerfTest.hpp"
#include "Performance/PerformanceReport.hpp"
#include "Log.hpp"

PerfTest::PerfTest(std::string&& name) noexcept :
	name(std::move(name))
{}

const std::string& PerfTest::getName() const
{
	return name;
}

bool PerfTest::isSkipped(const std::regex& filter) const
{
	return not std::regex_match(name, filter);
}


TimedTest::TimedTest(
	std::string&& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit
) noexcept:
	PerfTest(std::move(name)),
	limit(limit)
{}

void TimedTest::setup()
{}

void TimedTest::preTask()
{}

void TimedTest::postTask()
{}

void TimedTest::cleanup()
{}

size_t TimedTest::getTestCount() const
{
	return 1;
}

void TimedTest::runWarmUp()
{
	for (uint8_t i = 0; i < 10; ++i)
	{
		preTask();
		task();
		postTask();
	}
}

std::chrono::nanoseconds TimedTest::runCounted(const uint64_t repetitions)
{
	auto totalTime = std::chrono::nanoseconds(0);

	setup();
	runWarmUp();
	for (uint64_t i = 0; i < repetitions; ++i)
	{
		preTask();
		const auto start = std::chrono::high_resolution_clock::now();
		task();
		const auto end = std::chrono::high_resolution_clock::now();
		postTask();

		totalTime += end - start;
	}
	cleanup();

	const auto avgTime = totalTime / repetitions;
	return avgTime;
}

std::chrono::nanoseconds TimedTest::runTimed(std::chrono::nanoseconds minTime)
{
	auto totalTime = std::chrono::nanoseconds(0);
	uint64_t repetitions = 0;

	setup();
	runWarmUp();
	while (minTime.count() > 0)
	{
		preTask();
		const auto start = std::chrono::high_resolution_clock::now();
		task();
		const auto end = std::chrono::high_resolution_clock::now();
		postTask();

		const auto elapsed = end - start;
		totalTime += elapsed;
		minTime -= std::max(elapsed, std::chrono::nanoseconds(1));
		++repetitions;
	}
	cleanup();

	const auto avgTime = totalTime / repetitions;
	return avgTime;
}

std::chrono::nanoseconds TimedTest::run(PerformanceReport& report)
{
	LogBase::hide();

	const auto avgTime = std::holds_alternative<uint64_t>(limit) ?
		runCounted(std::get<uint64_t>(limit)) :
		runTimed(std::get<std::chrono::nanoseconds>(limit));

	LogBase::unhide();

	report.add(getName(), avgTime);

	return avgTime;
}

bool TimedTest::isSkipped(const std::regex& filter) const
{
	bool isInactive = std::holds_alternative<uint64_t>(limit) ?
		std::get<uint64_t>(limit) == 0 :
		std::get<std::chrono::nanoseconds>(limit).count() <= 0;

	return isInactive || PerfTest::isSkipped(filter);
}


PerfTestGroup::PerfTestGroup(
	std::string&& name,
	const std::regex& filter
) noexcept :
	PerfTest(std::move(name)),
	filter(filter)
{}

size_t PerfTestGroup::getTestCount() const
{
	return testCount;
}

std::chrono::nanoseconds PerfTestGroup::run(PerformanceReport& report)
{
	std::chrono::nanoseconds totalTime(0);

	Log(this).info("{0}: Running {1} sub-tests...", getName(), testCount);
	LogBase::nest();

	for (size_t i = 0; i < tests.size(); ++i)
	{
		// Setups
		if (tests[i]->getTestCount() == 0)
		{
			tests[i]->run(report);
			continue;
		}

		Log(this).info("\rRunning {0}...", tests[i]->getName());

		LogBase::nest();
		const auto time = tests[i]->run(report);
		LogBase::unnest();

		totalTime += time;
		const auto timeInMs = time.count() / 1'000'000.0;

		Log(this).info("\rTest {0} took {1}ms on average.", tests[i]->getName(), std::format("{:f}", timeInMs));
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
	return testCount == 0; // Skip empty groups
}
