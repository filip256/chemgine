#include "Performance/EstimatorPerfTests.hpp"
#include "Spline.hpp"

LinearSplinePerfTest::LinearSplinePerfTest(
	std::string&& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	float_s(*generator) (const float_s),
	const uint32_t size,
	const float_s loss
) noexcept:
	TimedTest(std::move(name), limit),
	loss(loss),
	input(generateInput(generator, size))
{}

std::vector<std::pair<float_s, float_s>> LinearSplinePerfTest::generateInput(
	float_s(*generator) (const float_s), const uint32_t size)
{
	std::vector<std::pair<float_s, float_s>> input;
	input.reserve(size);

	const auto step = static_cast<float_s>(100.0 / (size - 1));
	for (float_s x = -50.0; x < 50.0; x += step)
		input.emplace_back(x, generator(x));
	input.emplace_back(50.0, generator(50.0));

	return input;
}

void LinearSplinePerfTest::setup()
{
	inputCopy = input;
}

void LinearSplinePerfTest::cleanup()
{
	inputCopy.clear();
}

void LinearSplinePerfTest::task()
{
	dontOptimize = static_cast<bool>(Spline<float_s>(std::move(inputCopy), loss).size());
}


EstimatorPerfTests::EstimatorPerfTests(
	std::string&& name,
	const std::regex& filter
) noexcept :
	PerfTestGroup(std::move(name), filter)
{
	registerTest<LinearSplinePerfTest>("lspline_quadratic", std::chrono::seconds(10),
		[](const float_s x) -> float_s { return x * x; }, 100'000, 0.0);
	registerTest<LinearSplinePerfTest>("lspline_quadratic_loss", std::chrono::seconds(5),
		[](const float_s x) -> float_s { return x * x; }, 100'000, 0.25);
	registerTest<LinearSplinePerfTest>("lspline_sin", std::chrono::seconds(5),
		[](const float_s x) -> float_s { return std::sin(x); }, 10'000, 0.0);
	registerTest<LinearSplinePerfTest>("lspline_sin_loss", std::chrono::seconds(5),
		[](const float_s x) -> float_s { return std::sin(x); }, 10'000, 0.5);
}
