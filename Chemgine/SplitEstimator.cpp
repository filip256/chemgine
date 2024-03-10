#include "SplitEstimator.hpp"

SplitEstimator::SplitEstimator(
	const EstimatorId id,
	const BaseEstimator& lower,
	const BaseEstimator& higher,
	const double threshold
) noexcept :
	BaseEstimator(id),
	lower(lower),
	higher(higher),
	threshold(threshold)
{}

double SplitEstimator::get(const double input) const
{
	return input < threshold ? lower.get(input) : higher.get(input);
}

SplitEstimator* SplitEstimator::clone() const
{
	return new SplitEstimator(*this);
}