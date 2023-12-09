#include "SplineApproximator.hpp"

SplineApproximator::SplineApproximator(
	const ApproximatorIdType id,
	const std::string& name,
	Spline<float>&& spline
) noexcept :
	BaseApproximator(id, name, ApproximatorType::SPLINE),
	spline(std::move(spline))
{}

double SplineApproximator::execute(const double input) const
{
	return spline.getLinearValueAt(input);
}