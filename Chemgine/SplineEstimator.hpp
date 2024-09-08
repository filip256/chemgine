#pragma once

#include "TypedEstimator.hpp"
#include "Spline.hpp"

class SplineEstimator : public BaseEstimator
{
private:
	const Spline<float> spline;

public:
	SplineEstimator(
		const EstimatorId id,
		Spline<float>&& spline
	) noexcept;

	SplineEstimator(
		const EstimatorId id,
		const Spline<float>& spline
	) noexcept;

	double get(const double input) const override final;

	bool isEquivalent(const BaseEstimator& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	SplineEstimator* clone() const override final;
};

template<Unit OutU, Unit... InUs>
class TypedSplineEstimator : public TypedEstimator<OutU, InUs...>
{
private:
	const Spline<float> spline;

public:
	TypedSplineEstimator(
		const EstimatorId id,
		Spline<float>&& spline
	) noexcept :
		BaseEstimator(id),
		spline(std::move(spline))
	{}

	TypedSplineEstimator(
		const EstimatorId id,
		const Spline<float>& spline
	) noexcept :
		BaseEstimator(id),
		spline(spline)
	{}

	virtual Amount<OutU> get(const Amount<InUs>... inputs) const
	{
		return spline.getLinearValueAt(std::get<0>(std::forward_as_tuple(inputs...)));
	}

	SplineEstimator* clone() const override final
	{
		return new SplineEstimator(*this);
	}
};
