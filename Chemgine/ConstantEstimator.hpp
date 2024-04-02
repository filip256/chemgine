#pragma once

#include "BaseEstimator.hpp"

class ConstantEstimator : public BaseEstimator
{
private:
	const double constant;

public:
	ConstantEstimator(
		const EstimatorId id,
		const double constant
	) noexcept;

	double get(const double input) const override final;
	double get(const double input1, const double input2) const;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	ConstantEstimator* clone() const override final;
};
