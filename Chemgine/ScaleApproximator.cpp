#include "ScaleApproximator.hpp"

ScaleApproximator::ScaleApproximator(
	const ApproximatorIdType id,
	const BaseApproximator& base,
	const double scale
) noexcept :
	BaseApproximator(id),
	base(base),
	scale(scale)
{}

double ScaleApproximator::get(const double input) const
{
	return scale * base.get(input);
}

double ScaleApproximator::get(const double input1, const double input2) const
{
	return scale * base.get(input1, input2);
}

ScaleApproximator* ScaleApproximator::clone() const
{
	return new ScaleApproximator(*this);
}
