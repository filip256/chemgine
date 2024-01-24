#include "LinearEstimator.hpp"

LinearEstimator::LinearEstimator(
	const EstimatorIdType id,
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

LinearEstimator* LinearEstimator::clone() const
{
	return new LinearEstimator(*this);
}
