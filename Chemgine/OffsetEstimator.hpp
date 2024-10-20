#pragma once

#include "BaseEstimator.hpp"

class OffsetEstimator : public BaseEstimator
{
private:
	const double offset;
	const BaseEstimator& base;

public:
	OffsetEstimator(
		const EstimatorId id,
		const BaseEstimator& base,
		const double offset
	) noexcept;

	double get(const double input) const override final;
	double get(const double input1, const double input2) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	OffsetEstimator* clone() const override final;
};
