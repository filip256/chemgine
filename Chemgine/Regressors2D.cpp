#include "Regressors2D.hpp"
#include "NumericUtils.hpp"
#include "Log.hpp"

#include <typeinfo>

bool Regressor2DBase::isEquivalent(const Regressor2DBase& other, const float_n) const
{
	return typeid(*this) == typeid(other);
}


LinearRegressor2D::LinearRegressor2D(
	const float_n paramX,
	const float_n shift
) noexcept :
	paramX(paramX),
	shift(shift)
{}

float_n LinearRegressor2D::get(const float_n input) const
{
	return input * paramX + shift;
}

std::vector<float_n> LinearRegressor2D::getParams() const
{
	return { paramX, shift };
}

bool LinearRegressor2D::isEquivalent(const Regressor2DBase& other, const float_n epsilon) const
{
	if (not Regressor2DBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return
		Utils::floatEqual(this->paramX, oth.paramX, epsilon) &&
		Utils::floatEqual(this->shift, oth.shift, epsilon);
}

LinearRegressor2D LinearRegressor2D::fit(
	const std::vector<std::pair<float_n, float_n>>& points
)
{
	const size_t n = points.size();
	if (n == 0)
	{
		Log<LinearRegressor2D>().warn("Insufficient data points for fitting regressor.");
		return LinearRegressor2D(0.0, std::numeric_limits<float_n>::quiet_NaN());
	}

	// fallback on constant
	if (n == 1)
		return LinearRegressor2D(0.0, points.front().second);

	// https://www.statology.org/linear-regression-by-hand/
	float_n sumX = 0.0, sumR = 0.0, sumXR = 0.0, sumX2 = 0.0;
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
