#pragma once

#include "EstimatorSpecifier.hpp"

#include <vector>

class OOLSplineEstimator
{
private:
	EstimatorSpecifier specifier;
	std::vector<std::pair<float, float>> values;

public:
	OOLSplineEstimator(
		EstimatorSpecifier&& specifier,
		std::vector<std::pair<float, float>>&& values
	) noexcept;
	OOLSplineEstimator(const OOLSplineEstimator&) = delete;
	OOLSplineEstimator(OOLSplineEstimator&&) = default;

	OOLSplineEstimator& operator=(OOLSplineEstimator&&) = default;
};
