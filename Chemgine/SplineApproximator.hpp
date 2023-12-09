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
		const std::string& name,
		Spline<float>&& spline
	) noexcept;

	double execute(const double input) const override final;
};