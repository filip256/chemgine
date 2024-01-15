#include "ConstantApproximator.hpp"

ConstantApproximator::ConstantApproximator(
	const ApproximatorIdType id,
	const double constant
) noexcept :
	BaseApproximator(id),
	constant(constant)
{}

double ConstantApproximator::get(const double input) const
{
	return constant;
}

double ConstantApproximator::get(const double input1, const double input2) const
{
	return constant;
}

ConstantApproximator* ConstantApproximator::clone() const
{
	return new ConstantApproximator(*this);
}
