#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <chrono>
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

	virtual std::chrono::nanoseconds run(PerformanceReport& report) = 0;

	virtual bool isSkipped(const std::regex& filter) const;
};


class PerfTestSetup : public PerfTest
{
protected:
	virtual void task() = 0;

public:
	using PerfTest::PerfTest;

	size_t getTestCount() const override final;

	std::chrono::nanoseconds run(PerformanceReport& report) override final;

	bool isSkipped(const std::regex& filter) const override final;
};


class TimedTest : public PerfTest
{
private:
	const std::variant<uint64_t, std::chrono::nanoseconds> limit;

	std::chrono::nanoseconds runCounted(const uint64_t repetitions);
	std::chrono::nanoseconds runTimed(std::chrono::nanoseconds minTime);

protected:
	virtual void setup();
	virtual void cleanup();
	virtual void task() = 0;

public:
	TimedTest(
		std::string&& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit
	) noexcept;
	TimedTest(const TimedTest&) = default;
	TimedTest(TimedTest&&) = default;
	virtual ~TimedTest() = default;

	size_t getTestCount() const override final;

	std::chrono::nanoseconds run(PerformanceReport& report) override final;

	bool isSkipped(const std::regex& filter) const override final;
};


class PerfTestGroup : public PerfTest
{
private:
	size_t testCount = 0;
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

	std::chrono::nanoseconds run(PerformanceReport& report) override;
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
	if constexpr (std::is_constructible_v<T, std::string&&, const std::regex&, Args...>)
		test = std::make_unique<T>(std::move(name), filter, std::forward<Args>(args)...);
	else
		test = std::make_unique<T>(std::move(name), std::forward<Args>(args)...);

	if (test->isSkipped(filter))
		return;

	testCount += test->getTestCount();
	tests.emplace_back(std::move(test));
}
