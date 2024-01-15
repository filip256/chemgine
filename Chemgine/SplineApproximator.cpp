#include "SplineApproximator.hpp"

SplineApproximator::SplineApproximator(
	const ApproximatorIdType id,
	Spline<float>&& spline
) noexcept :
	BaseApproximator(id),
	spline(std::move(spline))
{}

double SplineApproximator::get(const double input) const
{
	return spline.getLinearValueAt(input);
}

SplineApproximator* SplineApproximator::clone() const
{
	return new SplineApproximator(*this);
}