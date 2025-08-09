#pragma once

#include "global/Precision.hpp"
#include "estimators/EstimationMode.hpp"

#include <utility>
#include <vector>

class Regressor2DBase
{
public:
	Regressor2DBase() = default;
	Regressor2DBase(const Regressor2DBase&) = default;
	virtual ~Regressor2DBase() = default;

	virtual float_s get(const float_s input) const = 0;

	virtual std::vector<float_s> getParams() const = 0;

	virtual bool isEquivalent(const Regressor2DBase& other,
		const float_s epsilon = std::numeric_limits<float_s>::epsilon()
	) const;
};


class LinearRegressor2D : public Regressor2DBase
{
public:
	const float_s paramX;
	const float_s shift;

	LinearRegressor2D(
		const float_s paramX,
		const float_s shift
	) noexcept;
	LinearRegressor2D(const LinearRegressor2D&) = default;

	float_s get(const float_s input) const override final;

	std::vector<float_s> getParams() const override final;

	bool isEquivalent(const Regressor2DBase& other,
		const float_s epsilon = std::numeric_limits<float_s>::epsilon()
	) const override final;

	static LinearRegressor2D fit(const std::vector<std::pair<float_s, float_s>>& points);

	static constexpr EstimationMode Mode = EstimationMode::LINEAR;
};
