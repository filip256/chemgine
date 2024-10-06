#pragma once

#include "Casts.hpp"
#include "DataPoint.hpp"
#include "AffineEstimator.hpp"
#include "SplineEstimator.hpp"
#include "ConstantEstimator.hpp"
#include "Regression2DEstimator.hpp"
#include "Regression3DEstimator.hpp"
#include "EstimatorRepository.hpp"
#include "ImmutableSet.hpp"

class EstimatorFactory
{
private:
	EstimatorRepository& repository;

public:
	EstimatorFactory(EstimatorRepository& repository) noexcept;

	template<Unit OutU, Unit... InUs>
	EstimatorRef<OutU, InUs...> createConstant(
		const Amount<OutU> constant);

	template<Unit OutU, Unit... InUs>
	EstimatorRef<OutU, InUs...> createData(
		std::vector<DataPoint<OutU, InUs...>>&& dataPoints,
		const EstimationMode mode,
		const float maxCompressionLoss);

	template<Unit OutU, Unit InU>
	EstimatorRef<OutU, InU> createAffine(
		const EstimatorRef<OutU, InU>& base,
		const double vShift,
		const double hShift,
		const double scale);

	template<Unit OutU, Unit InU>
	EstimatorRef<OutU, InU> createAffine(
		const EstimatorRef<OutU, InU>& base,
		const DataPoint<OutU, InU> anchorPoint,
		const double hShift);

	template<Unit OutU, Unit InU>
	EstimatorRef<OutU, InU> createAffine(
		const EstimatorRef<OutU, InU>& base,
		const DataPoint<OutU, InU> rebasePoint);

	template<Unit OutU, Unit InU>
	EstimatorRef<OutU, InU> createLinearRegression(
		const float paramX, const float shift);

	template<Unit OutU, Unit InU1, Unit InU2>
	EstimatorRef<OutU, InU1, InU2> createLinearRegression(
		const float paramX, const float paramY, const float shift);
};


template<Unit OutU, Unit... InUs>
EstimatorRef<OutU, InUs...> EstimatorFactory::createConstant(
	const Amount<OutU> constant)
{
	return repository.add<ConstantEstimator<OutU, InUs...>>(constant);
}

template<Unit OutU, Unit... InUs>
EstimatorRef<OutU, InUs...> EstimatorFactory::createData(
	std::vector<DataPoint<OutU, InUs...>>&& dataPoints,
	const EstimationMode mode,
	const float maxCompressionLoss)
{
	const auto uniquePoints = ImmutableSet<DataPoint<OutU, InUs...>>::toSortedSetVector(std::move(dataPoints));

	// Constant ND
	if (uniquePoints.size() == 1)
		return createConstant<OutU, InUs...>(uniquePoints.front().output);

	const auto inputCount = sizeof...(InUs);
	if constexpr (inputCount == 1)
	{
		std::vector<std::pair<float, float>> points;
		points.reserve(uniquePoints.size());
		std::transform(uniquePoints.begin(), uniquePoints.end(), std::back_inserter(points),
			[](const DataPoint<OutU, InUs...>& p)
			{
				return std::pair(std::get<0>(p.inputs).asStd(), p.output.asStd());
			}); 

		// Linear 2D
		if (uniquePoints.size() == 2)
		{
			if(mode == EstimationMode::LINEAR)
				return repository.add<Regression2DEstimator<LinearRegressor2D, OutU, InUs...>>(LinearRegressor2D::fit(points), mode);
		}

		// Spline 2D
		return repository.add<SplineEstimator<OutU, InUs...>>(Spline<float>(std::move(points), maxCompressionLoss), mode);
	}
	else if constexpr (inputCount == 2)
	{
		std::vector<std::tuple<float, float, float>> points;
		points.reserve(uniquePoints.size());
		std::transform(uniquePoints.begin(), uniquePoints.end(), std::back_inserter(points),
			[](const DataPoint<OutU, InUs...>& p)
			{
				return std::tuple(std::get<0>(p.inputs).asStd(), std::get<1>(p.inputs).asStd(), p.output.asStd());
			});

		// Linear 3D
		if (mode == EstimationMode::LINEAR)
			return repository.add<Regression3DEstimator<LinearRegressor3D, OutU, InUs...>>(LinearRegressor3D::fit(points), mode);
	}

	Log(this).fatal("Unsupported estimator data type.");
}


template<Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createAffine(
	const EstimatorRef<OutU, InU>& base,
	const double vShift,
	const double hShift,
	const double scale)
{
	// no transform, return base
	if (Utils::equal(vShift, 0.0) && Utils::equal(hShift, 0.0) && Utils::equal(scale, 1.0))
		return EstimatorRef<OutU, InU>(base);

	// constant folding
	if (const auto constBase = base.cast<ConstantEstimator<OutU, InU>>())
	{
		return createConstant<OutU, InU>(constBase->get(0.0) * scale + vShift);
	}

	// linear folding
	if (const auto linearBase = base.cast<Regression2DEstimator<LinearRegressor2D, OutU, InU>>())
	{
		const auto& regressor = linearBase->getRegressor();
		return repository.add<Regression2DEstimator<LinearRegressor2D, OutU, InU>>(
			LinearRegressor2D(
				/* paramX= */ regressor.paramX * scale,
				/* shift= */ (regressor.paramX * hShift - regressor.shift) * scale + vShift),
			EstimationMode::LINEAR);
	}

	// affine folding
	if (const auto affineBase = base.cast<AffineEstimator<OutU, InU>>())
	{
		return repository.add<AffineEstimator<OutU, InU>>(
			affineBase->getBase(),
			/* vShift= */ - (affineBase->hShift + hShift) * affineBase->scale * scale + vShift * (scale + 1.0),
			/* hShift= */ 0.0,
			/* scale= */ affineBase->scale * scale);
	}

	return repository.add<AffineEstimator<OutU, InU>>(base, vShift, hShift, scale);
}

template<Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createAffine(
	const EstimatorRef<OutU, InU>& base,
	const DataPoint<OutU, InU> anchorPoint,
	const double hShift)
{
	if (Utils::equal(hShift, 0.0))
	{
		// f(X) = base(X) - base(Xr) + Yr
		const auto vShift = anchorPoint.output - base->get(std::get<0>(anchorPoint.inputs));
		return createAffine(base, vShift.asStd(), 0.0, 1.0);
	}

	// f(X) = base(X - Xr + Xh) - base(Xh) + Yr
	const auto vShift = anchorPoint.output - base->get(hShift);
	const auto newHShift = std::get<0>(anchorPoint.inputs) - hShift;
	return createAffine(base, vShift.asStd(), newHShift.asStd(), 1.0);
}

template<Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createAffine(
	const EstimatorRef<OutU, InU>& base,
	const DataPoint<OutU, InU> rebasePoint)
{
	const auto hShift = std::get<0>(rebasePoint.inputs);
	const auto scale = rebasePoint.output;
	return createAffine(base, 0.0, hShift.asStd(), scale.asStd());
}

template<Unit OutU, Unit InU>
EstimatorRef<OutU, InU> EstimatorFactory::createLinearRegression(
	const float paramX, const float shift)
{
	return repository.add<Regression2DEstimator<LinearRegressor2D, OutU, InU>>(
		LinearRegressor2D(paramX, shift),
		EstimationMode::LINEAR);
}

template<Unit OutU, Unit InU1, Unit InU2>
EstimatorRef<OutU, InU1, InU2> EstimatorFactory::createLinearRegression(
	const float paramX, const float paramY, const float shift)
{
	return repository.add<Regression3DEstimator<LinearRegressor3D, OutU, InU1, InU2>>(
		LinearRegressor3D(paramX, paramY, shift),
		EstimationMode::LINEAR);
}
