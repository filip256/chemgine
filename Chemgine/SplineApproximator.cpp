#include "SplineApproximator.hpp"

SplineApproximator::SplineApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	Spline<float>&& spline
) noexcept :
	BaseApproximator(id, name, ApproximatorType::SPLINE),
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