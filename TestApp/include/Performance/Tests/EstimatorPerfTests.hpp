#pragma once

#include "Performance/PerfTest.hpp"
#include "Precision.hpp"

class LinearSplinePerfTest : public TimedTest
{
private:
	volatile bool dontOptimize;

	const bool loss;
	const std::vector<std::pair<float_s, float_s>> input;
	std::vector<std::pair<float_s, float_s>> inputCopy;

	static std::vector<std::pair<float_s, float_s>> generateInput(
		float_s(*generator) (const float_s), const uint32_t size);

public:
	LinearSplinePerfTest(
		std::string&& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		float_s(*generator) (const float_s),
		const uint32_t size,
		const float_s loss
	) noexcept;

	void preTask() override final;
	void task() override final;
	void postTask() override final;
};


class EstimatorPerfTests : public PerfTestGroup
{
public:
	EstimatorPerfTests(
		std::string&& name,
		const std::regex& filter
	) noexcept;
};
