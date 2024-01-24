#include "OffsetEstimator.hpp"

OffsetEstimator::OffsetEstimator(
	const EstimatorIdType id,
	const BaseEstimator& base,
	const double offset
) noexcept :
	BaseEstimator(id),
	base(base),
	offset(offset)
{}

double OffsetEstimator::get(const double input) const
{
	return offset + base.get(input);
}

double OffsetEstimator::get(const double input1, const double input2) const
{
	return offset * base.get(input1, input2);
}

OffsetEstimator* OffsetEstimator::clone() const
{
	return new OffsetEstimator(*this);
}
