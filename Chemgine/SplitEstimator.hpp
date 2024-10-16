#pragma once

#include "BaseEstimator.hpp"

class SplitEstimator : public BaseEstimator
{
private:
	const double threshold;
	const BaseEstimator& lower;
	const BaseEstimator& higher;

public:
	SplitEstimator(
		const EstimatorId id,
		const BaseEstimator& lower,
		const BaseEstimator& higher,
		const double threshold
	) noexcept;

	double get(const double input) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	SplitEstimator* clone() const override final;
};
