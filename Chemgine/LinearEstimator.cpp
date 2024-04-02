#include "LinearEstimator.hpp"

LinearEstimator::LinearEstimator(
	const EstimatorId id,
	const double scale,
	const double offset
) noexcept :
	BaseEstimator(id),
	scale(scale),
	offset(offset)
{}

double LinearEstimator::get(const double input) const
{
	return scale * input + offset;
}

bool LinearEstimator::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	if (BaseEstimator::isEquivalent(other, epsilon) == false)
		return false;

	const auto& o = static_cast<const LinearEstimator&>(other);
	return 
		std::abs(this->scale - o.scale) <= epsilon &&
		std::abs(this->offset - o.offset) <= epsilon;
}

LinearEstimator* LinearEstimator::clone() const
{
	return new LinearEstimator(*this);
}
