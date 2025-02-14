#include "Unit/UnitTest.hpp"
#include "Log.hpp"

#include <chrono>

UnitTest::UnitTest(std::string&& name) noexcept :
	name(std::move(name))
{}

const std::string& UnitTest::getName() const
{
	return name;
}

size_t UnitTest::getTestCount() const
{
	return 1;
}

bool UnitTest::isSkipped(const std::regex& filter) const
{
	return not std::regex_match(name, filter);
}


UnitTestGroup::UnitTestGroup(
	std::string&& name,
	const std::regex& filter
) noexcept:
	UnitTest(std::move(name)),
	filter(filter)
{}

size_t UnitTestGroup::getTestCount() const
{
	return testCount;
}

bool UnitTestGroup::run()
{
	bool allPassed = true;

	Log(this).info("{0}: Running {1} sub-tests...", getName(), testCount);
	LogBase::nest();

	for (size_t i = 0; i < tests.size(); ++i)
	{
		// Setups
		if (tests[i]->getTestCount() == 0)
		{
			tests[i]->run();
			continue;
		}

		Log(this).info("\rRunning {0}...", tests[i]->getName());

		LogBase::nest();
		const auto start = std::chrono::high_resolution_clock::now();
		const auto success = tests[i]->run();
		const auto end = std::chrono::high_resolution_clock::now();
		LogBase::unnest();

		const auto timeInMs = (end - start).count() / 1000000.0;

		if (success)
			Log(this).info("\rTest {0} passed ({1}ms).", tests[i]->getName(), std::format("{:f}", timeInMs));
		else
		{
			allPassed = false;
			Log(this).error("\rTest {0} failed ({1}ms).", tests[i]->getName(), std::format("{:f}", timeInMs));
		}

		Log(this).info("\0");
	}

	LogBase::unnest();
	return allPassed;
}

bool UnitTestGroup::isSkipped(const std::regex&) const
{
	return testCount == 0; // Skip empty groups
}
