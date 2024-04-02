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

bool SplineEstimator::isEquivalent(const BaseEstimator& other, const double epsilon) const
{
	if (BaseEstimator::isEquivalent(other, epsilon) == false)
		return false;

	const auto& o = static_cast<const SplineEstimator&>(other);
	return this->spline.isEquivalent(o.spline, epsilon);
}

SplineEstimator* SplineEstimator::clone() const
{
	return new SplineEstimator(*this);
}
