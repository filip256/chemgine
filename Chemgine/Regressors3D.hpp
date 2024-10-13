#pragma once

#include "Precision.hpp"

#include <tuple>
#include <vector>
#include <optional>

class Regressor3DBase
{
public:
	Regressor3DBase() = default;
	Regressor3DBase(const Regressor3DBase&) = default;
	virtual ~Regressor3DBase() = default;

	virtual float_n get(const float_n input1, const float_n input2) const = 0;

	virtual std::vector<float_n> getParams() const = 0;

	virtual bool isEquivalent(const Regressor3DBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const;
};


class LinearRegressor3D : public Regressor3DBase
{
public:
	const float_n paramX;
	const float_n paramY;
	const float_n shift;

	LinearRegressor3D(
		const float_n paramX,
		const float_n paramY,
		const float_n shift
	) noexcept;
	LinearRegressor3D(const LinearRegressor3D&) = default;

	float_n get(const float_n input1, const float_n input2) const override final;

	std::vector<float_n> getParams() const override final;

	bool isEquivalent(const Regressor3DBase& other,
		const float_n epsilon = std::numeric_limits<float_n>::epsilon()
	) const override final;

	static LinearRegressor3D fit(const std::vector<std::tuple<float_n, float_n, float_n>>& points);
};
