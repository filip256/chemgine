#pragma once

#include "common/TestSetup.hpp"
#include "TimingResult.hpp"
#include "utils/Process.hpp"
#include "utils/Meta.hpp"

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <regex>

class PerformanceReport;

class PerfTest
{
private:
	const std::string name;

public:
	PerfTest(std::string&& name) noexcept;
	PerfTest(const PerfTest&) = default;
	PerfTest(PerfTest&&) = default;
	virtual ~PerfTest() = default;

	const std::string& getName() const;
	virtual size_t getTestCount() const = 0;
	virtual std::chrono::nanoseconds getEstimatedRunTime() const = 0;

	virtual TimingResult run(PerformanceReport& report) = 0;

	virtual bool isSkipped(const std::regex& filter) const;
};


template<typename SetupT>
class PerfTestSetup final : public PerfTest
{
	static_assert(std::is_base_of_v<TestSetup, SetupT>,
		"PerfTestSetup: SetupT must be a TestSetup derived type.");

private:
	SetupT setup;

public:
	PerfTestSetup(
		std::string&& name,
		SetupT&& setup
	) noexcept;

	using SetupType = SetupT;

	size_t getTestCount() const override final;
	std::chrono::nanoseconds getEstimatedRunTime() const override final;

	TimingResult run(PerformanceReport& report) override final;

	bool isSkipped(const std::regex& filter) const override final;
};

template<typename SetupT>
PerfTestSetup<SetupT>::PerfTestSetup(
	std::string&& name,
	SetupT&& setup
) noexcept :
	PerfTest(std::move(name)),
	setup(std::move(setup))
{}

template<typename SetupT>
size_t PerfTestSetup<SetupT>::getTestCount() const
{
	return 0; // Test setups should not be counted
}

template<typename SetupT>
std::chrono::nanoseconds PerfTestSetup<SetupT>::getEstimatedRunTime() const
{
	return std::chrono::milliseconds(1);
}

template<typename SetupT>
TimingResult PerfTestSetup<SetupT>::run(PerformanceReport&)
{
	setup.run();
	return TimingResult(std::chrono::nanoseconds(0), std::chrono::nanoseconds(0));
}

template<typename SetupT>
bool PerfTestSetup<SetupT>::isSkipped(const std::regex&) const
{
	return false; // Never skip setups
}


class TimedTest : public PerfTest
{
private:
	const std::variant<size_t, std::chrono::nanoseconds> limit;

	static std::chrono::nanoseconds WarmUpTime;

	void runWarmUp();
	TimingResult runCounted(const size_t repetitions);
	TimingResult runTimed(std::chrono::nanoseconds minTime);

	OS::ExecutionConfig static stabilizeExecution();
	void static restoreExecution(const OS::ExecutionConfig normalProperties);

protected:
	virtual void setup();
	virtual void preTask();
	virtual void task() = 0;
	virtual void postTask();
	virtual void cleanup();

public:
	TimedTest(
		std::string&& name,
		const std::variant<size_t, std::chrono::nanoseconds> limit
	) noexcept;
	TimedTest(const TimedTest&) = default;
	TimedTest(TimedTest&&) = default;
	virtual ~TimedTest() = default;

	size_t getTestCount() const override final;
	std::chrono::nanoseconds getEstimatedRunTime() const override final;

	TimingResult run(PerformanceReport& report) override final;

	bool isSkipped(const std::regex& filter) const override final;
};


class PerfTestGroup : public PerfTest
{
private:
	size_t testCount = 0;
	std::chrono::nanoseconds estimatedRunTime = std::chrono::nanoseconds(0);
	std::vector<std::unique_ptr<PerfTest>> tests;
	const std::regex& filter;

protected:
	template<typename T, typename... Args>
	void registerTest(std::string&& name, Args&&... args);

public:
	PerfTestGroup(
		std::string&& name,
		const std::regex& filter
	) noexcept;

	size_t getTestCount() const override final;
	std::chrono::nanoseconds getEstimatedRunTime() const override final;

	TimingResult run(PerformanceReport& report) override;
	PerformanceReport generateReport();

	bool isSkipped(const std::regex& filter) const override final;
};

template<typename T, typename... Args>
void PerfTestGroup::registerTest(std::string&& name, Args&&... args)
{
	static_assert(std::is_base_of_v<PerfTest, T>,
		"PerfTestGroup: T must be a PerfTest derived type.");

	name = getName() + '.' + name; // Append owning group name

	std::unique_ptr<PerfTest> test;
	if constexpr (utils::is_specialization_of_v<T, PerfTestSetup>)
		test = std::make_unique<T>(std::move(name), T::SetupType(std::forward<Args>(args)...));
	else if constexpr (std::is_constructible_v<T, std::string&&, const std::regex&, Args...>)
		test = std::make_unique<T>(std::move(name), filter, std::forward<Args>(args)...);
	else
		test = std::make_unique<T>(std::move(name), std::forward<Args>(args)...);

	if (test->isSkipped(filter))
		return;

	testCount += test->getTestCount();
	estimatedRunTime += test->getEstimatedRunTime() + std::chrono::milliseconds(1);
	tests.emplace_back(std::move(test));
}
