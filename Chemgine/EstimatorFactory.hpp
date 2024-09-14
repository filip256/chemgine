#pragma once

#include "Casts.hpp"
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
	const UnitizedEstimator<OutU, InUs...>* createData(
		std::vector<DataPoint<OutU, InUs...>>&& dataPoints,
		const EstimationMode mode,
		const float maxCompressionLoss);

	template<Unit OutU, Unit InU>
	const UnitizedEstimator<OutU, InU>* createAffine(
		const UnitizedEstimator<OutU, InU>& base,
		const double vShift,
		const double hShift,
		const double scale);

	template<Unit OutU, Unit InU>
	const UnitizedEstimator<OutU, InU>* createAffine(
		const UnitizedEstimator<OutU, InU>& base,
		const DataPoint<OutU, InU> anchorPoint,
		const double hShift);

	template<Unit OutU, Unit InU>
	const UnitizedEstimator<OutU, InU>* createAffine(
		const UnitizedEstimator<OutU, InU>& base,
		const DataPoint<OutU, InU> rebasePoint);
};


template<Unit OutU, Unit... InUs>
const UnitizedEstimator<OutU, InUs...>* EstimatorFactory::createData(
	std::vector<DataPoint<OutU, InUs...>>&& dataPoints,
	const EstimationMode mode,
	const float maxCompressionLoss)
{
	const auto uniquePoints = ImmutableSet<DataPoint<OutU, InUs...>>::toSortedSetVector(std::move(dataPoints));

	// Constant ND
	if (uniquePoints.size() == 1)
		return &repository.add<ConstantEstimator<OutU, InUs...>>(uniquePoints.front().output);

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
				return &repository.add<Regression2DEstimator<LinearRegressor2D, OutU, InUs...>>(LinearRegressor2D::fit(points), mode);
		}

		// Spline 2D
		return &repository.add<SplineEstimator<OutU, InUs...>>(Spline<float>(std::move(points), maxCompressionLoss), mode);
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
			return &repository.add<Regression3DEstimator<LinearRegressor3D, OutU, InUs...>>(LinearRegressor3D::fit(points), mode);
	}

	Log<EstimatorFactory>().fatal("Unsupported estimator data type.");
	return nullptr;
}


template<Unit OutU, Unit InU>
const UnitizedEstimator<OutU, InU>* EstimatorFactory::createAffine(
	const UnitizedEstimator<OutU, InU>& base,
	const double vShift,
	const double hShift,
	const double scale)
{
	// no transform, return base
	if (Utils::equal(vShift, 0.0) && Utils::equal(hShift, 0.0) && Utils::equal(scale, 1.0))
		return &base;

	// constant folding
	if (const auto constBase = final_cast<ConstantEstimator<OutU, InU>>(base))
	{
		return &repository.add<ConstantEstimator<OutU, InU>>(constBase->get(0.0) * scale + vShift);
	}

	// linear folding
	if (const auto linearBase = final_cast<Regression2DEstimator<LinearRegressor2D, OutU, InU>>(base))
	{
		const auto& regressor = linearBase->getRegressor();
		return &repository.add<Regression2DEstimator<LinearRegressor2D, OutU, InU>>(LinearRegressor2D(
				regressor.paramX * hShift * scale, regressor.shift * scale + vShift), EstimationMode::LINEAR);
	}

	return &repository.add<AffineEstimator<OutU, InU>>(base, vShift, hShift, scale);
}

template<Unit OutU, Unit InU>
const UnitizedEstimator<OutU, InU>* EstimatorFactory::createAffine(
	const UnitizedEstimator<OutU, InU>& base,
	const DataPoint<OutU, InU> anchorPoint,
	const double hShift)
{
	if (Utils::equal(hShift, 0.0))
	{
		// f(X) = base(X) - base(Xr) + Yr
		const auto vShift = anchorPoint.output - base.get(std::get<0>(anchorPoint.inputs));
		return createAffine(base, vShift.asStd(), 0.0, 1.0);
	}

	// f(X) = base(X - Xr + Xh) - base(Xh) + Yr
	const auto vShift = anchorPoint.output - base.get(hShift);
	const auto newHShift = std::get<0>(anchorPoint.inputs) - hShift;
	return createAffine(base, vShift.asStd(), newHShift.asStd(), 1.0);
}

template<Unit OutU, Unit InU>
const UnitizedEstimator<OutU, InU>* EstimatorFactory::createAffine(
	const UnitizedEstimator<OutU, InU>& base,
	const DataPoint<OutU, InU> rebasePoint)
{
	const auto hShift = std::get<0>(rebasePoint.inputs);
	const auto scale = rebasePoint.output;
	return createAffine(base, 0.0, hShift.asStd(), scale.asStd());
}
