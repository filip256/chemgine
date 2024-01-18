#pragma once

#include "BaseApproximator.hpp"

class OffsetApproximator : public BaseApproximator
{
private:
	const double offset;
	const BaseApproximator& base;

public:
	OffsetApproximator(
		const ApproximatorIdType id,
		const BaseApproximator& base,
		const double offset
	) noexcept;

	double get(const double input) const override final;
	double get(const double input1, const double input2) const override final;

	OffsetApproximator* clone() const override final;
};