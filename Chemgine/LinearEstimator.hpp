#pragma once

#include "BaseEstimator.hpp"

class LinearEstimator : public BaseEstimator
{
private:
	const double scale, offset;

public:
	LinearEstimator(
		const 
		EstimatorId id,
		const double scale,
		const double offset
	) noexcept;

	double get(const double input) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	LinearEstimator* clone() const override final;
};
