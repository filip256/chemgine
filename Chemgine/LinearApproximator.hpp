#pragma once

#include "BaseApproximator.hpp"

class LinearApproximator : public BaseApproximator
{
private:
	const double scale, offset;

public:
	LinearApproximator(
		const ApproximatorIdType id,
		const double scale,
		const double offset
	) noexcept;

	double get(const double input) const override final;

	LinearApproximator* clone() const override final;
};