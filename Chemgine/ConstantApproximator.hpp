#pragma once

#include "BaseApproximator.hpp"

class ConstantApproximator : public BaseApproximator
{
private:
	const double constant;

public:
	ConstantApproximator(
		const ApproximatorIdType id,
		const double constant
	) noexcept;

	double get(const double input) const override final;
	double get(const double input1, const double input2) const;

	ConstantApproximator* clone() const override final;
};