#include "OOLSplineEstimator.hpp"

OOLSplineEstimator::OOLSplineEstimator(
	EstimatorSpecifier&& specifier,
	std::vector<std::pair<float, float>>&& values
) noexcept :
	specifier(std::move(specifier)),
	values(std::move(values))
{}
