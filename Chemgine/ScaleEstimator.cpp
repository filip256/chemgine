#include "ScaleEstimator.hpp"

ScaleEstimator::ScaleEstimator(
	const EstimatorId id,
	const BaseEstimator& base,
	const double scale
) noexcept :
	BaseEstimator(id),
	base(base),
	scale(scale)
{}

double ScaleEstimator::get(const double input) const
{
	return scale * base.get(input);
}

double ScaleEstimator::get(const double input1, const double input2) const
{
	return scale * base.get(input1, input2);
}

ScaleEstimator* ScaleEstimator::clone() const
{
	return new ScaleEstimator(*this);
}
