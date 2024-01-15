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

ScaleApproximator* ScaleApproximator::clone() const
{
	return new ScaleApproximator(*this);
}
