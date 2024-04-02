#include "OffsetEstimator.hpp"

OffsetEstimator::OffsetEstimator(
	const EstimatorId id,
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

bool OffsetEstimator::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	if (BaseEstimator::isEquivalent(other, epsilon) == false)
		return false;

	const auto& o = static_cast<const OffsetEstimator&>(other);
	return std::abs(this->offset - o.offset) <= epsilon &&
		this->base.isEquivalent(o.base);
}

OffsetEstimator* OffsetEstimator::clone() const
{
	return new OffsetEstimator(*this);
}
