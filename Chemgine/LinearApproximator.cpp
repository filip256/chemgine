#include "LinearApproximator.hpp"

LinearApproximator::LinearApproximator(
	const ApproximatorIdType id,
	const double scale,
	const double offset
) noexcept :
	BaseApproximator(id),
	scale(scale),
	offset(offset)
{}

double LinearApproximator::get(const double input) const
{
	return scale * input + offset;
}

LinearApproximator* LinearApproximator::clone() const
{
	return new LinearApproximator(*this);
}
