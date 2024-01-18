#pragma once

#include "BaseApproximator.hpp"

class ScaleApproximator : public BaseApproximator
{
private:
	const double scale;
	const BaseApproximator& base;

public:
	ScaleApproximator(
		const ApproximatorIdType id,
		const BaseApproximator& base,
		const double scale
	) noexcept;

	double get(const double input) const override final;
	double get(const double input1, const double input2) const override final;

	ScaleApproximator* clone() const override final;
};