#include "Regressors2D.hpp"
#include "NumericUtils.hpp"
#include "Log.hpp"

#include <typeinfo>

bool Regressor2DBase::isEquivalent(const Regressor2DBase& other, const float) const
{
	return typeid(*this) == typeid(other);
}


LinearRegressor2D::LinearRegressor2D(
	const float paramX,
	const float shift
) noexcept :
	paramX(paramX),
	shift(shift)
{}

float LinearRegressor2D::get(const float input) const
{
	return input * paramX + shift;
}

bool LinearRegressor2D::isEquivalent(const Regressor2DBase& other, const float epsilon) const
{
	if (not Regressor2DBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return
		Utils::equal(this->paramX, oth.paramX, epsilon) &&
		Utils::equal(this->shift, oth.shift, epsilon);
}

LinearRegressor2D LinearRegressor2D::fit(
	const std::vector<std::pair<float, float>>& points
)
{
	const size_t n = points.size();
	if (n == 0)
	{
		Log<LinearRegressor2D>().warn("Insufficient data points for fitting regressor.");
		return LinearRegressor2D(0.0, std::numeric_limits<float>::quiet_NaN());
	}

	// fallback on constant
	if (n == 1)
		return LinearRegressor2D(0.0, points.front().second);

	// https://www.statology.org/linear-regression-by-hand/
	double sumX = 0.0, sumR = 0.0, sumXR = 0.0, sumX2 = 0.0;
	for (size_t i = 0; i < n; ++i)
	{
		const auto x = points[i].first;
		const auto r = points[i].second;

		sumX += x;
		sumR += r;
		sumXR += x * r;
		sumX2 += x * x;
	}

	const auto variance = (n * sumX2 - sumX * sumX);
	const auto paramX = (n * sumXR - sumX * sumR) / variance;
	const auto shift = (sumR * sumX2 - sumX * sumXR) / variance;

	return LinearRegressor2D(paramX, shift);
}
