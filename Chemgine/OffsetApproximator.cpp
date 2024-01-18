#include "OffsetApproximator.hpp"

OffsetApproximator::OffsetApproximator(
	const ApproximatorIdType id,
	const BaseApproximator& base,
	const double offset
) noexcept :
	BaseApproximator(id),
	base(base),
	offset(offset)
{}

double OffsetApproximator::get(const double input) const
{
	return offset + base.get(input);
}

double OffsetApproximator::get(const double input1, const double input2) const
{
	return offset * base.get(input1, input2);
}

OffsetApproximator* OffsetApproximator::clone() const
{
	return new OffsetApproximator(*this);
}
