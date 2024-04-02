#pragma once

#include "BaseEstimator.hpp"

class ScaleEstimator : public BaseEstimator
{
private:
	const double scale;
	const BaseEstimator& base;

public:
	ScaleEstimator(
		const EstimatorId id,
		const BaseEstimator& base,
		const double scale
	) noexcept;

	double get(const double input) const override final;
	double get(const double input1, const double input2) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	ScaleEstimator* clone() const override final;
};
