#pragma once

#include "BaseEstimator.hpp"
#include "Spline.hpp"

class SplineEstimator : public BaseEstimator
{
private:
	const Spline<float> spline;

public:
	SplineEstimator(
		const EstimatorIdType id,
		Spline<float>&& spline
	) noexcept;

	double get(const double input) const override final;

	SplineEstimator* clone() const override final;
};