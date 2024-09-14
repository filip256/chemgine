#pragma once

#include "UnitizedEstimator.hpp"
#include "Spline.hpp"

template<Unit OutU, Unit InU>
class SplineEstimator : public UnitizedEstimator<OutU, InU>
{
private:
	const Spline<float> spline;

public:
	SplineEstimator(
		const EstimatorId id,
		Spline<float>&& spline,
		const EstimationMode mode
	) noexcept;

	SplineEstimator(
		const EstimatorId id,
		const Spline<float>& spline,
		const EstimationMode mode
	) noexcept;

	Amount<OutU> get(const Amount<InU> input) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	SplineEstimator* clone() const override final;
};

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	Spline<float>&& spline,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU>(id, mode),
	spline(std::move(spline))
{}

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>::SplineEstimator(
	const EstimatorId id,
	const Spline<float>& spline,
	const EstimationMode mode
) noexcept :
	SplineEstimator(id, Utils::copy(spline), mode)
{}

template<Unit OutU, Unit InU>
Amount<OutU> SplineEstimator<OutU, InU>::get(const Amount<InU> input) const
{
	return spline.getLinearValueAt(input.asStd());
}

template<Unit OutU, Unit InU>
bool SplineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->spline.isEquivalent(oth.spline, epsilon);
}

template<Unit OutU, Unit InU>
SplineEstimator<OutU, InU>* SplineEstimator<OutU, InU>::clone() const
{
	return new SplineEstimator(*this);
}
