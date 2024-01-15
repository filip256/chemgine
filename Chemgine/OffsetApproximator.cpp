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

OffsetApproximator* OffsetApproximator::clone() const
{
	return new OffsetApproximator(*this);
}
