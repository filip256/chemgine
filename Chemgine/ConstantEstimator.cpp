#include "ConstantEstimator.hpp"

ConstantEstimator::ConstantEstimator(
	const EstimatorId id,
	const double constant
) noexcept :
	BaseEstimator(id),
	constant(constant)
{}

double ConstantEstimator::get(const double input) const
{
	return constant;
}

double ConstantEstimator::get(const double input1, const double input2) const
{
	return constant;
}

bool ConstantEstimator::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	if (BaseEstimator::isEquivalent(other, epsilon) == false)
		return false;

	const auto& o = static_cast<const ConstantEstimator&>(other);
	return std::abs(this->constant - o.constant) <= epsilon;
}

ConstantEstimator* ConstantEstimator::clone() const
{
	return new ConstantEstimator(*this);
}
