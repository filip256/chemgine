#include "structs/Regressors3D.hpp"

#include "io/Log.hpp"
#include "utils/Math.hpp"
#include "utils/Numeric.hpp"

#include <typeinfo>

bool Regressor3DBase::isEquivalent(const Regressor3DBase& other, const float_s) const
{
    return typeid(*this) == typeid(other);
}

LinearRegressor3D::LinearRegressor3D(const float_s paramX, const float_s paramY, const float_s shift) noexcept :
    paramX(paramX),
    paramY(paramY),
    shift(shift)
{}

float_s LinearRegressor3D::get(const float_s input1, const float_s input2) const
{
    return input1 * paramX + input2 * paramY + shift;
}

std::vector<float_s> LinearRegressor3D::getParams() const { return {paramX, paramY, shift}; }

bool LinearRegressor3D::isEquivalent(const Regressor3DBase& other, const float_s epsilon) const
{
    if (not Regressor3DBase::isEquivalent(other, epsilon))
        return false;

    const auto& oth = static_cast<decltype(*this)&>(other);
    return utils::floatEqual(this->paramX, oth.paramX, epsilon) &&
           utils::floatEqual(this->paramY, oth.paramY, epsilon) &&
           utils::floatEqual(this->shift, oth.shift, epsilon);
}

LinearRegressor3D LinearRegressor3D::fit(const std::vector<std::tuple<float_s, float_s, float_s>>& points)
{
    const size_t n = points.size();
    if (n == 0) {
        Log<LinearRegressor3D>().warn("Insufficient data points for fitting regressor.");
        return LinearRegressor3D(0.0, 0.0, std::numeric_limits<float_s>::quiet_NaN());
    }

    // fallback on constant
    if (n == 1)
        return LinearRegressor3D(0.0, 0.0, std::get<2>(points.front()));

    // fallback on 2D regression on one of the inputs
    if (n == 2) {
        const auto x1 = std::get<0>(points.front());
        const auto x2 = std::get<0>(points.back());
        const auto y1 = std::get<1>(points.front());
        const auto y2 = std::get<1>(points.back());
        const auto r1 = std::get<2>(points.front());
        const auto r2 = std::get<2>(points.back());

        if (utils::floatEqual(y1, y2)) {
            const auto temp = utils::getSlopeAndIntercept(x1, r1, x2, r2);
            return LinearRegressor3D(temp.first, 0.0, temp.second);
        }

        if (utils::floatEqual(x1, x2)) {
            const auto temp = utils::getSlopeAndIntercept(y1, r1, y2, r2);
            return LinearRegressor3D(0.0, temp.first, temp.second);
        }

        Log<LinearRegressor3D>().warn("Insufficient data points for fitting regressor.");
        return LinearRegressor3D(0.0, 0.0, std::numeric_limits<float_s>::quiet_NaN());
    }

    // https://www.statology.org/multiple-linear-regression-by-hand/
    float_s sumX = 0.0, sumY = 0.0, sumR = 0.0;
    float_s sumXR = 0.0, sumYR = 0.0, sumXY = 0.0;
    float_s sumX2 = 0.0, sumY2 = 0.0;

    for (size_t i = 0; i < n; ++i) {
        const auto x = std::get<0>(points[i]);
        const auto y = std::get<1>(points[i]);
        const auto r = std::get<2>(points[i]);

        sumX  += x;
        sumY  += y;
        sumR  += r;
        sumXR += x * r;
        sumYR += y * r;
        sumXY += x * y;
        sumX2 += x * x;
        sumY2 += y * y;
    }

    const auto meanX = sumX / n;
    const auto meanY = sumY / n;
    const auto meanR = sumR / n;

    const auto normXR = sumXR - (sumX * sumR) / n;
    const auto normYR = sumYR - (sumY * sumR) / n;
    const auto normXY = sumXY - (sumX * sumY) / n;
    const auto normX2 = sumX2 - (sumX * sumX) / n;
    const auto normY2 = sumY2 - (sumY * sumY) / n;

    const auto variance = (normX2 * normY2 - normXY * normXY);
    const auto paramX   = (normY2 * normXR - normXY * normYR) / variance;
    const auto paramY   = (normX2 * normYR - normXY * normXR) / variance;
    const auto shift    = meanR - paramX * meanX - paramY * meanY;

    return LinearRegressor3D(paramX, paramY, shift);
}
