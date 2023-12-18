#include "OffsetApproximator.hpp"

OffsetApproximator::OffsetApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	const BaseApproximator& base,
	const double offset
) noexcept :
	BaseApproximator(id, name, base.type),
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
