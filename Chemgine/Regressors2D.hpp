#pragma once

#include "Precision.hpp"
#include "EstimationMode.hpp"

#include <utility>
#include <vector>

class Regressor2DBase
{
public:
	Regressor2DBase() = default;
	Regressor2DBase(const Regressor2DBase&) = default;
	virtual ~Regressor2DBase() = default;

	virtual float_n get(const float_n input) const = 0;

	virtual std::vector<float_n> getParams() const = 0;

	virtual bool isEquivalent(const Regressor2DBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const;
};


class LinearRegressor2D : public Regressor2DBase
{
public:
	const float_n paramX;
	const float_n shift;

	LinearRegressor2D(
		const float_n paramX,
		const float_n shift
	) noexcept;
	LinearRegressor2D(const LinearRegressor2D&) = default;

	float_n get(const float_n input) const override final;

	std::vector<float_n> getParams() const override final;

	bool isEquivalent(const Regressor2DBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const override final;

	static LinearRegressor2D fit(const std::vector<std::pair<float_n, float_n>>& points);

	static constexpr EstimationMode Mode = EstimationMode::LINEAR;
};
