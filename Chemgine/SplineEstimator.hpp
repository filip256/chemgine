#pragma once

#include "BaseEstimator.hpp"
#include "Spline.hpp"

class SplineEstimator : public BaseEstimator
{
private:
	const Spline<float> spline;

public:
	SplineEstimator(
		const EstimatorId id,
		Spline<float>&& spline
	) noexcept;

	SplineEstimator(
		const EstimatorId id,
		const Spline<float>& spline
	) noexcept;

	double get(const double input) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	SplineEstimator* clone() const override final;
};
