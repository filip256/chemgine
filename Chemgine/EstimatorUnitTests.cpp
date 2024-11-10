#include "EstimatorUnitTests.hpp"

Estimator2DUnitTestBase::Estimator2DUnitTestBase(
	std::string&& name,
	float_s(*generator) (const float_s)
) noexcept :
	UnitTest(std::move(name)),
	generator(generator)
{}

Amount<Unit::ANY> Estimator2DUnitTestBase::generateAt(const float_s x)
{
	return generator(x);
}

std::vector<DataPoint<Unit::ANY, Unit::ANY>> Estimator2DUnitTestBase::generateData(
	const float_s minX, const float_s maxX, const size_t size) const
{
	std::vector<DataPoint<Unit::ANY, Unit::ANY>> data;
	data.reserve(size);

	const float_s step = (maxX - minX) / (size - 1);
	for (float_s x = minX; x < maxX; x += step)
		data.emplace_back(generator(x), x);

	data.emplace_back(generator(maxX), maxX);
	return data;
}


DataEstimator2DUnitTest::DataEstimator2DUnitTest(
	std::string&& name,
	float_s(*generator) (const float_s),
	const EstimationMode mode,
	const float_s loss,
	const uint32_t inputSize,
	const float_s testMinX,
	const float_s testMaxX,
	const float_h testThreshold
) noexcept :
	Estimator2DUnitTestBase(std::move(name), generator),
	factory(repository),
	mode(mode),
	loss(loss),
	inputSize(inputSize),
	testMinX(testMinX),
	testMaxX(testMaxX),
	testThreshold(testThreshold)
{}

bool DataEstimator2DUnitTest::run()
{
	const auto testSize = (inputSize - 1) * 4;
	const auto estimator = factory.createData(generateData(testMinX, testMaxX, inputSize), mode, loss);

	float_h error = 0.0;
	size_t n = 0;

	// Input:  ----|-----------|----
	// Checks: -^--^--^--^--^--^--^-
	const float_s step = (testMaxX - testMinX) / testSize;
	for (float_s x = testMinX; x < testMaxX; x += step)
	{
		const auto ref = generateAt(x).asStd();
		const auto act = estimator->get(x).asStd();
		error += std::abs(ref - act);
		++n;
	}

	const auto ref = generateAt(testMaxX).asStd();
	const auto act = estimator->get(testMaxX).asStd();
	error += std::abs(ref - act);
	++n;
	
	error /= n;
	if (error <= testThreshold)
	{
		Log(this).trace("UnitTest passed with error: {0} (MAE).", std::format("{:e}", error));
		return true;
	}

	Log(this).error("Error: {0} (MAE) exceeded the test threshold: {1}.", std::format("{:e}", error), std::format("{:e}", testThreshold));
	return error <= testThreshold;
}


Estimator3DUnitTestBase::Estimator3DUnitTestBase(
	std::string&& name,
	float_s(*generator) (const float_s, const float_s)
) noexcept:
	UnitTest(std::move(name)),
	generator(generator)
{}

Amount<Unit::ANY> Estimator3DUnitTestBase::generateAt(const float_s x1, const float_s x2)
{
	return generator(x1, x2);
}

std::vector<DataPoint<Unit::ANY, Unit::ANY, Unit::ANY>> Estimator3DUnitTestBase::generateData(
	const float_s minX1, const float_s maxX1,
	const float_s minX2, const float_s maxX2,
	const size_t size) const
{
	std::vector<DataPoint<Unit::ANY, Unit::ANY, Unit::ANY>> data;
	data.reserve(size * size);

	const float_s step1 = (maxX1 - minX1) / (size - 1);
	const float_s step2 = (maxX2 - minX2) / (size - 1);
	for (float_s x1 = minX1; x1 < maxX1; x1 += step1)
		for (float_s x2 = minX2; x2 < maxX2; x2 += step2)
			data.emplace_back(generator(x1, x2), x1, x2);

	data.emplace_back(generator(maxX1, maxX2), maxX1, maxX2);
	return data;
}


DataEstimator3DUnitTest::DataEstimator3DUnitTest(
	std::string&& name,
	float_s(*generator) (const float_s, const float_s),
	const EstimationMode mode,
	const float_s loss,
	const uint32_t inputSize,
	const float_s testMinX1,
	const float_s testMaxX1,
	const float_s testMinX2,
	const float_s testMaxX2,
	const float_h testThreshold
) noexcept :
	Estimator3DUnitTestBase(std::move(name), generator),
	factory(repository),
	mode(mode),
	loss(loss),
	inputSize(inputSize),
	testMinX1(testMinX1),
	testMaxX1(testMaxX1),
	testMinX2(testMinX2),
	testMaxX2(testMaxX2),
	testThreshold(testThreshold)
{}

bool DataEstimator3DUnitTest::run()
{
	const auto testSize = (inputSize - 1) * 4;
	const auto estimator = factory.createData(generateData(testMinX1, testMaxX1, testMinX2, testMaxX2, inputSize), mode, loss);

	float_h error = 0.0;
	size_t n = 0;

	// Input:  ----|-----------|----
	// Checks: -^--^--^--^--^--^--^-
	const float_s step1 = (testMaxX1 - testMinX1) / testSize;
	const float_s step2 = (testMaxX2 - testMinX2) / testSize;
	for (float_s x1 = testMinX1; x1 < testMaxX1; x1 += step1)
		for (float_s x2 = testMinX2; x2 < testMaxX2; x2 += step2)
		{
			const auto ref = generateAt(x1, x2).asStd();
			const auto act = estimator->get(x1, x2).asStd();
			error += std::abs(ref - act);
			++n;
		}

	for (float_s x2 = testMinX2; x2 < testMaxX2; x2 += step2)
	{
		const auto ref = generateAt(testMaxX1, x2).asStd();
		const auto act = estimator->get(testMaxX1, x2).asStd();
		error += std::abs(ref - act);
		++n;
	}

	const auto ref = generateAt(testMaxX1, testMaxX2).asStd();
	const auto act = estimator->get(testMaxX1, testMaxX2).asStd();
	error += std::abs(ref - act);
	++n;

	error /= n;
	if (error <= testThreshold)
	{
		Log(this).trace("UnitTest passed with error: {0} (MAE).", std::format("{:e}", error));
		return true;
	}

	Log(this).error("Error: {0} (MAE) exceeded the test threshold: {1}.", std::format("{:e}", error), std::format("{:e}", testThreshold));
	return error <= testThreshold;
}


EstimatorUnitTests::EstimatorUnitTests(
	std::string&& name,
	const std::regex& filter
) noexcept :
	UnitTestGroup(std::move(name), filter)
{
	registerTest<DataEstimator2DUnitTest>("const_2D",
		[](const float_s x) -> float_s { return 7.0f; }, EstimationMode::LINEAR,
		0.0f, 1000, -100.0f, 100.0f, 0.0);
	registerTest<DataEstimator2DUnitTest>("linear_steep_2D",
		[](const float_s x) -> float_s { return 7.5f * x - 7.2f; }, EstimationMode::LINEAR,
		0.0f, 1000, -100.0f, 100.0f, 1e-05);
	registerTest<DataEstimator2DUnitTest>("linear_negative_2D",
		[](const float_s x) -> float_s { return -0.99f * x + 1.02f; }, EstimationMode::LINEAR,
		0.0f, 1000, -100.0f, 100.0f, 1e-05);
	registerTest<DataEstimator2DUnitTest>("quadratic_steep_2D",
		[](const float_s x) -> float_s { return 1.17f * x * x + 1.6f * x + 17.4f; }, EstimationMode::LINEAR,
		0.0f, 1000, -10.0f, 10.0f, 1e-04);
	registerTest<DataEstimator2DUnitTest>("quadratic_negative_2D",
		[](const float_s x) -> float_s { return -0.02f * x * x + 7.6f * x - 1.5f; }, EstimationMode::LINEAR,
		0.0f, 1000, -50.0f, 50.0f, 1e-04);
	registerTest<DataEstimator2DUnitTest>("quadratic_loss_2D",
		[](const float_s x) -> float_s { return -0.02f * x * x + 7.6f * x - 1.5f; }, EstimationMode::LINEAR,
		1e-04f, 1000, -50.0f, 50.0f, 1e+00);
	registerTest<DataEstimator2DUnitTest>("cubic_steep_2D",
		[](const float_s x) -> float_s { return 1.2f * x * x * x + 6.9f * x * x + 1.1f * x + 3.25f; }, EstimationMode::LINEAR,
		0.0f, 1000, -20.0f, 20.0f, 1e-02);
	registerTest<DataEstimator2DUnitTest>("cubic_negative_2D",
		[](const float_s x) -> float_s { return -0.025f * x * x * x - 0.5f * x * x + 7.1f * x - 1.5f; }, EstimationMode::LINEAR,
		0.0f, 1000, -5.0f, 5.0f, 1e-03);
	registerTest<DataEstimator2DUnitTest>("cubic_loss_2D",
		[](const float_s x) -> float_s { return -0.025f * x * x * x - 0.5f * x * x + 7.1f * x - 1.5f; }, EstimationMode::LINEAR,
		1e-04f, 1000, -5.0f, 5.0f, 1e-01);
	registerTest<DataEstimator2DUnitTest>("sin_2D",
		[](const float_s x) -> float_s { return std::sin(x); }, EstimationMode::LINEAR,
		0.0f, 1000, -10.0f, 10.0f, 1e-04);
	registerTest<DataEstimator2DUnitTest>("sin_loss_2D",
		[](const float_s x) -> float_s { return std::sin(x); }, EstimationMode::LINEAR,
		1e-02f, 1000, -10.0f, 10.0f, 1e-01);
	registerTest<DataEstimator2DUnitTest>("tetration_1_2D",
		[](const float_s x) -> float_s { return std::pow(x, x); }, EstimationMode::LINEAR,
		0.0f, 1000, 0.0f, 2.0f, 1e-05);
	registerTest<DataEstimator2DUnitTest>("tetration_1_loss_2D",
		[](const float_s x) -> float_s { return std::pow(x, x); }, EstimationMode::LINEAR,
		1e-03f, 1000, 0.0f, 2.0f, 1e-01);
	registerTest<DataEstimator2DUnitTest>("fuzz_2D",
		[](const float_s x) -> float_s { return std::sin(x * x * x); }, EstimationMode::LINEAR,
		0.0f, 100, 2.0f, 20.0f, 1e+00);

	registerTest<DataEstimator3DUnitTest>("const_3D",
		[](const float_s x1, const float_s x2) -> float_s { return 2.0f; }, EstimationMode::LINEAR,
		0.0f, 100, -100.0f, 100.0f, -100.0f, 100.0f, 0.0);
	registerTest<DataEstimator3DUnitTest>("linear_3D",
		[](const float_s x1, const float_s x2) -> float_s { return x1 * 1.2f - x2 * 0.9f + 4.2f; }, EstimationMode::LINEAR,
		0.0f, 100, -100.0f, 100.0f, -100.0f, 100.0f, 1e-02);
	registerTest<DataEstimator3DUnitTest>("bilinear_3D",
		[](const float_s x1, const float_s x2) -> float_s { return x1 * x2 * 1.1f - 2.27f; }, EstimationMode::LINEAR,
		0.0f, 100, -30.0f, 30.0f, -30.0f, 30.0f, 1e+03);
	registerTest<DataEstimator3DUnitTest>("quasilinear_3D",
		[](const float_s x1, const float_s x2) -> float_s { return x1 * x1 * 1.2f - x2 * 0.8f + 1.25f; }, EstimationMode::LINEAR,
		0.0f, 100, -30.0f, 30.0f, -30.0f, 30.0f, 1e+03);
	registerTest<DataEstimator3DUnitTest>("quadratic_3D",
		[](const float_s x1, const float_s x2) -> float_s { return x1 * x1 * 1.3f + x2 * x2 * 1.7f + x1 * x2 + x1 * 0.9f + x2 * 0.7f - 1.25f; }, EstimationMode::LINEAR,
		0.0f, 100, -30.0f, 30.0f, -30.0f, 30.0f, 1e+04);
	registerTest<DataEstimator3DUnitTest>("cubic_3D",
		[](const float_s x1, const float_s x2) -> float_s { return x1 * x1 * x1 * 1.3f + x2 * x2 * x2 * 1.2f + x1 * x1 * x2 * 1.5f + x1 * x1 * 0.5f + x2 * x2 * 0.4f + x1 * x2 * 0.6f + x1 * 0.1f + x2 * 0.2f + 0.02f; }, EstimationMode::LINEAR,
		0.0f, 100, -30.0f, 30.0f, -30.0f, 30.0f, 1e+05);
	registerTest<DataEstimator3DUnitTest>("div_3D",
		[](const float_s x1, const float_s x2) -> float_s { return x1 / (x2 != 0.0f ? x2 : std::numeric_limits<float_s>::min()); }, EstimationMode::LINEAR,
		0.0f, 100, -30.0f, 30.0f, -30.0f, 30.0f, 1e+03);
	registerTest<DataEstimator3DUnitTest>("fuzz_3D",
		[](const float_s x1, const float_s x2) -> float_s { return std::powf(std::sin(x1 * x2 / std::tan(x2 * x2)), 2); }, EstimationMode::LINEAR,
		0.0f, 20, -20.0f, 20.0f, -20.0f, 20.0f, 1e+00);
}
