#pragma once

#include "BaseApproximator.hpp"
#include "Spline.hpp"

class SplineApproximator : public BaseApproximator
{
private:
	const Spline<float> spline;

public:
	SplineApproximator(
		const ApproximatorIdType id,
		Spline<float>&& spline
	) noexcept;

	double get(const double input) const override final;

	SplineApproximator* clone() const override final;
};