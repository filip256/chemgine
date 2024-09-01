#pragma once

#include "EstimatorSpecifier.hpp"
#include "SplineEstimator.hpp"
#include "DynamicAmount.hpp"

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

	template<Unit InT, Unit OutT>
	std::optional<SplineEstimator> convert() const;
};

template<Unit InT, Unit OutT>
std::optional<SplineEstimator> OOLSplineEstimator::convert() const
{
	//std::vector<std::pair<float, float>> convertedValues;
	//convertedValues.reserve(values.size());

	//for (size_t i = 0; i < values.size(); ++i)
	//{
	//	const auto in = DynamicAmount(values[i].first, specifier.inUnit).to<InT>();
	//	const auto out = DynamicAmount(values[i].second, specifier.outUnit).to<OutT>();

	//	if (in.has_value() == false || out.has_value() == false)
	//		return std::nullopt;

	//	convertedValues.emplace(in->asStr(), out->asStr());
	//}

	//return SplineEstimator(100, Spline<float>(std::move(convertedValues)));

	return SplineEstimator(100, Spline<float>(values));
}
