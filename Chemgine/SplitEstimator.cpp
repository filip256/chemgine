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

bool SplitEstimator::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	if (BaseEstimator::isEquivalent(other, epsilon) == false)
		return false;

	const auto& o = static_cast<const SplitEstimator&>(other);
	return
		std::abs(this->threshold - o.threshold) <= epsilon &&
		this->lower.isEquivalent(o.lower) &&
		this->higher.isEquivalent(o.higher);
}

SplitEstimator* SplitEstimator::clone() const
{
	return new SplitEstimator(*this);
}