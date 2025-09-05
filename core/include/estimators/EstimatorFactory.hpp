#pragma once

#include "data/values/DataPoint.hpp"
#include "estimators/EstimatorRepository.hpp"
#include "estimators/kinds/AffineEstimator.hpp"
#include "estimators/kinds/ConstantEstimator.hpp"
#include "estimators/kinds/RegressionEstimator.hpp"
#include "estimators/kinds/SplineEstimator.hpp"
#include "structs/ImmutableSet.hpp"
#include "structs/Regressors2D.hpp"
#include "structs/Regressors3D.hpp"
#include "utils/Build.hpp"
#include "utils/Casts.hpp"

class EstimatorFactory
{
private:
    EstimatorRepository& repository;

public:
    EstimatorFactory(EstimatorRepository& repository) noexcept;

    template <Unit OutU, Unit... InUs>
    EstimatorRef<OutU, InUs...> createConstant(const Amount<OutU> constant);

    template <Unit OutU, Unit... InUs>
    EstimatorRef<OutU, InUs...> createData(
        std::vector<DataPoint<OutU, InUs...>>&& dataPoints,
        const EstimationMode                    mode,
        const float_s                           maxCompressionLoss);

    template <Unit OutU, Unit InU>
    EstimatorRef<OutU, InU>
    createAffine(const EstimatorRef<OutU, InU>& base, const float_s vShift, const float_s hShift, const float_s scale);

    template <Unit OutU, Unit InU>
    EstimatorRef<OutU, InU>
    createAffine(const EstimatorRef<OutU, InU>& base, const DataPoint<OutU, InU> anchorPoint, const float_s hShift);

    template <Unit OutU, Unit InU>
    EstimatorRef<OutU, InU> createAffine(const EstimatorRef<OutU, InU>& base, const DataPoint<OutU, InU> rebasePoint);

    template <Unit OutU, Unit InU>
    EstimatorRef<OutU, InU> createLinearRegression(const float_s paramX, const float_s shift);

    template <Unit OutU, Unit InU1, Unit InU2>
    EstimatorRef<OutU, InU1, InU2>
    createLinearRegression(const float_s paramX, const float_s paramY, const float_s shift);
};

template <Unit OutU, Unit... InUs>
EstimatorRef<OutU, InUs...> EstimatorFactory::createConstant(const Amount<OutU> constant)
{
    return repository.add<ConstantEstimator<OutU, InUs...>>(constant);
}

template <Unit OutU, Unit... InUs>
EstimatorRef<OutU, InUs...> EstimatorFactory::createData(
    std::vector<DataPoint<OutU, InUs...>>&& dataPoints, const EstimationMode mode, const float_s maxCompressionLoss)
{
    const auto uniquePoints = ImmutableSet<DataPoint<OutU, InUs...>>::toSortedSetVector(std::move(dataPoints));

    // Constant AND
    if (uniquePoints.size() == 1)
        return createConstant<OutU, InUs...>(uniquePoints.front().output);

    const auto inputCount = sizeof...(InUs);
    if constexpr (inputCount == 1) {
        std::vector<std::pair<float_s, float_s>> points;
        points.reserve(uniquePoints.size());
        std::transform(
            uniquePoints.begin(),
            uniquePoints.end(),
            std::back_inserter(points),
            [](const DataPoint<OutU, InUs...>& p) {
            return std::pair(std::get<0>(p.inputs).asStd(), p.output.asStd());
        });

        // Linear 2D
        if (uniquePoints.size() == 2) {
            if (mode == EstimationMode::LINEAR)
                return repository.add<RegressionEstimator<LinearRegressor2D, OutU, InUs...>>(
                    LinearRegressor2D::fit(points));
        }

        // Spline 2D
        return repository.add<SplineEstimator<OutU, InUs...>>(
            Spline<float_s>(std::move(points), maxCompressionLoss), mode);
    }
    else if constexpr (inputCount == 2) {
        std::vector<std::tuple<float_s, float_s, float_s>> points;
        points.reserve(uniquePoints.size());
        std::transform(
            uniquePoints.begin(),
            uniquePoints.end(),
            std::back_inserter(points),
            [](const DataPoint<OutU, InUs...>& p) {
            return std::tuple(std::get<0>(p.inputs).asStd(), std::get<1>(p.inputs).asStd(), p.output.asStd());
        });

        // Linear 3D
        if (mode == EstimationMode::LINEAR)
            return repository.add<RegressionEstimator<LinearRegressor3D, OutU, InUs...>>(
                LinearRegressor3D::fit(points));
    }
    else
        static_assert(utils::always_false_v<sizeof...(InUs)>, "Unsupported estimator data type.");

    CHG_UNREACHABLE();
}

template <Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createAffine(
    const EstimatorRef<OutU, InU>& base, const float_s vShift, const float_s hShift, const float_s scale)
{
    // no transform, return base
    if (utils::floatEqual(vShift, 0.0f) && utils::floatEqual(hShift, 0.0f) && utils::floatEqual(scale, 1.0f))
        return EstimatorRef<OutU, InU>(base);

    // constant folding
    if (const auto constBase = base.template cast<ConstantEstimator<OutU, InU>>()) {
        return createConstant<OutU, InU>(constBase->get(0.0f) * scale + vShift);
    }

    // linear folding
    if (const auto linearBase = base.template cast<RegressionEstimator<LinearRegressor2D, OutU, InU>>()) {
        const auto& regressor = linearBase->getRegressor();
        return repository.add<RegressionEstimator<LinearRegressor2D, OutU, InU>>(LinearRegressor2D(
            /* paramX= */ regressor.paramX * scale,
            /* shift= */ (regressor.paramX * hShift - regressor.shift) * scale + vShift));
    }

    // affine folding
    if (const auto affineBase = base.template cast<AffineEstimator<OutU, InU>>()) {
        return repository.add<AffineEstimator<OutU, InU>>(
            affineBase->getBase(),
            /* vShift= */ -(affineBase->hShift + hShift) * affineBase->scale * scale + vShift * (scale + 1.0f),
            /* hShift= */ 0.0f,
            /* scale= */ affineBase->scale * scale);
    }

    return repository.add<AffineEstimator<OutU, InU>>(base, vShift, hShift, scale);
}

template <Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createAffine(
    const EstimatorRef<OutU, InU>& base, const DataPoint<OutU, InU> anchorPoint, const float_s hShift)
{
    if (utils::floatEqual(hShift, 0.0f)) {
        // f(X) = base(X) - base(Xr) + Yr
        const auto vShift = anchorPoint.output - base->get(std::get<0>(anchorPoint.inputs));
        return createAffine(base, vShift.asStd(), 0.0f, 1.0f);
    }

    // f(X) = base(X - Xr + Xh) - base(Xh) + Yr
    const auto vShift    = anchorPoint.output - base->get(hShift);
    const auto newHShift = std::get<0>(anchorPoint.inputs) - hShift;
    return createAffine(base, vShift.asStd(), newHShift.asStd(), 1.0f);
}

template <Unit OutU, Unit InU>
EstimatorRef<OutU, InU>
EstimatorFactory::createAffine(const EstimatorRef<OutU, InU>& base, const DataPoint<OutU, InU> rebasePoint)
{
    const auto hShift = std::get<0>(rebasePoint.inputs);
    const auto scale  = rebasePoint.output;
    return createAffine(base, 0.0f, hShift.asStd(), scale.asStd());
}

template <Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createLinearRegression(const float_s paramX, const float_s shift)
{
    return repository.add<RegressionEstimator<LinearRegressor2D, OutU, InU>>(LinearRegressor2D(paramX, shift));
}

template <Unit OutU, Unit InU1, Unit InU2>
EstimatorRef<OutU, InU1, InU2>
EstimatorFactory::createLinearRegression(const float_s paramX, const float_s paramY, const float_s shift)
{
    return repository.add<RegressionEstimator<LinearRegressor3D, OutU, InU1, InU2>>(
        LinearRegressor3D(paramX, paramY, shift));
}
