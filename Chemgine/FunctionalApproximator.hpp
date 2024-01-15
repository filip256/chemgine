#pragma once

#include "BaseApproximator.hpp"

#include <string>

class FunctionalApproximator : public BaseApproximator
{
private:
	double (*const function)(double);

public:
	FunctionalApproximator(
		const ApproximatorIdType id,
		double(*function)(double)
	) noexcept;

	double get(const double input) const override final;

	FunctionalApproximator* clone() const override final;
};