#pragma once

#include "Unit/UnitTest.hpp"
#include "Precision.hpp"
#include "Performance/PerfTest.hpp"

class TimingUnitTest : public UnitTest
{
	class DummyPerfTest : public TimedTest
	{
	private:
		const std::chrono::nanoseconds waitTime;

	protected:
		void task() override final;

	public:
		DummyPerfTest(
			const std::variant<uint64_t, std::chrono::nanoseconds> limit,
			const std::chrono::nanoseconds waitTime
		) noexcept;

		std::chrono::nanoseconds getWaitTime() const;
	};

private:
	DummyPerfTest perfTest;
	const float_h threshold;

public:
	TimingUnitTest(
		const std::string& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		const std::chrono::nanoseconds waitTime,
		const float_h threshold
	) noexcept;

	bool run() override final;
};


class TimingUnitTests : public UnitTestGroup
{
public:
	TimingUnitTests(
		std::string&& name,
		const std::regex& filter
	) noexcept;
};
