#pragma once

#include <utility>
#include <vector>

class Regressor2DBase
{
public:
	Regressor2DBase() = default;
	Regressor2DBase(const Regressor2DBase&) = default;
	virtual ~Regressor2DBase() = default;

	virtual float get(const float input) const = 0;

	virtual bool isEquivalent(const Regressor2DBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const;
};


class LinearRegressor2D : public Regressor2DBase
{
public:
	const float paramX;
	const float shift;

	LinearRegressor2D(
		const float paramX,
		const float shift
	) noexcept;
	LinearRegressor2D(const LinearRegressor2D&) = default;

	float get(const float input) const override final;

	bool isEquivalent(const Regressor2DBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const override final;

	static LinearRegressor2D fit(const std::vector<std::pair<float, float>>& points);
};
