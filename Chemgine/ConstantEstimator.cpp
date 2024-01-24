#include "ConstantEstimator.hpp"

ConstantEstimator::ConstantEstimator(
	const EstimatorIdType id,
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

ConstantEstimator* ConstantEstimator::clone() const
{
	return new ConstantEstimator(*this);
}
