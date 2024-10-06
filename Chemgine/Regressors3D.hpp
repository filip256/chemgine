#pragma once

#include <tuple>
#include <vector>
#include <optional>

class Regressor3DBase
{
public:
	Regressor3DBase() = default;
	Regressor3DBase(const Regressor3DBase&) = default;
	virtual ~Regressor3DBase() = default;

	virtual float get(const float input1, const float input2) const = 0;

	virtual std::vector<float> getParams() const = 0;

	virtual bool isEquivalent(const Regressor3DBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const;
};


class LinearRegressor3D : public Regressor3DBase
{
public:
	const float paramX;
	const float paramY;
	const float shift;

	LinearRegressor3D(
		const float paramX,
		const float paramY,
		const float shift
	) noexcept;
	LinearRegressor3D(const LinearRegressor3D&) = default;

	float get(const float input1, const float input2) const override final;

	std::vector<float> getParams() const override final;

	bool isEquivalent(const Regressor3DBase& other,
		const float epsilon = std::numeric_limits<float>::epsilon()
	) const override final;

	static LinearRegressor3D fit(const std::vector<std::tuple<float, float, float>>& points);
};
