#include "SplineEstimator.hpp"

SplineEstimator::SplineEstimator(
	const EstimatorIdType id,
	Spline<float>&& spline
) noexcept :
	BaseEstimator(id),
	spline(std::move(spline))
{}

double SplineEstimator::get(const double input) const
{
	return spline.getLinearValueAt(input);
}

SplineEstimator* SplineEstimator::clone() const
{
	return new SplineEstimator(*this);
}