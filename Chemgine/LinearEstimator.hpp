#pragma once

#include "BaseEstimator.hpp"

class LinearEstimator : public BaseEstimator
{
private:
	const double scale, offset;

public:
	LinearEstimator(
		const 
		EstimatorIdType id,
		const double scale,
		const double offset
	) noexcept;

	double get(const double input) const override final;

	LinearEstimator* clone() const override final;
};