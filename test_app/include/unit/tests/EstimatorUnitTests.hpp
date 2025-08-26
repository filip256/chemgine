#pragma once

#include "data/values/DataPoint.hpp"
#include "estimators/EstimatorFactory.hpp"
#include "global/Precision.hpp"
#include "unit/UnitTest.hpp"

class Estimator2DUnitTestBase : public UnitTest
{
private:
    float_s (*generator)(const float_s);

protected:
    Estimator2DUnitTestBase(std::string&& name, float_s (*generator)(const float_s)) noexcept;

    Amount<Unit::ANY> generateAt(const float_s x);

    std::vector<DataPoint<Unit::ANY, Unit::ANY>>
    generateData(const float_s minX, const float_s maxX, const size_t size) const;
};

class DataEstimator2DUnitTest : public Estimator2DUnitTestBase
{
private:
    EstimatorRepository  repository;
    EstimatorFactory     factory;
    const EstimationMode mode;
    const float_s        loss;
    const uint32_t       inputSize;
    const float_s        testMinX;
    const float_s        testMaxX;
    const float_h        testThreshold;

public:
    DataEstimator2DUnitTest(
        std::string&&        name,
        float_s              (*generator)(const float_s),
        const EstimationMode mode,
        const float_s        loss,
        const uint32_t       inputSize,
        const float_s        testMinX,
        const float_s        testMaxX,
        const float_h        testThreshold) noexcept;

    bool run() override final;
};

class Estimator3DUnitTestBase : public UnitTest
{
private:
    float_s (*generator)(const float_s, const float_s);

protected:
    Estimator3DUnitTestBase(std::string&& name, float_s (*generator)(const float_s, const float_s)) noexcept;

    Amount<Unit::ANY> generateAt(const float_s x1, const float_s x2);

    std::vector<DataPoint<Unit::ANY, Unit::ANY, Unit::ANY>> generateData(
        const float_s minX1, const float_s maxX1, const float_s minX2, const float_s maxX2, const size_t size) const;
};

class DataEstimator3DUnitTest : public Estimator3DUnitTestBase
{
private:
    EstimatorRepository  repository;
    EstimatorFactory     factory;
    const EstimationMode mode;
    const float_s        loss;
    const uint32_t       inputSize;
    const float_s        testMinX1;
    const float_s        testMaxX1;
    const float_s        testMinX2;
    const float_s        testMaxX2;
    const float_h        testThreshold;

public:
    DataEstimator3DUnitTest(
        std::string&&        name,
        float_s              (*generator)(const float_s, const float_s),
        const EstimationMode mode,
        const float_s        loss,
        const uint32_t       inputSize,
        const float_s        testMinX1,
        const float_s        testMaxX1,
        const float_s        testMinX2,
        const float_s        testMaxX2,
        const float_h        testThreshold) noexcept;

    bool run() override final;
};

class EstimatorUnitTests : public UnitTestGroup
{
public:
    EstimatorUnitTests(std::string&& name, const std::regex& filter) noexcept;
};
