#include "SplineEstimator.hpp"

SplineEstimator::SplineEstimator(
	const EstimatorId id,
	Spline<float>&& spline
) noexcept :
	BaseEstimator(id),
	spline(std::move(spline))
{}

SplineEstimator::SplineEstimator(
	const EstimatorId id,
	const Spline<float>& spline
) noexcept :
	BaseEstimator(id),
	spline(spline)
{}

double SplineEstimator::get(const double input) const
{
	return spline.getLinearValueAt(input);
}

SplineEstimator* SplineEstimator::clone() const
{
	return new SplineEstimator(*this);
}