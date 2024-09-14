#pragma once

#include "UnitizedEstimator.hpp"

template<Unit OutU, Unit InU>
class AffineEstimator : public UnitizedEstimator<OutU, InU>
{
private:
	const double vShift = 0.0;
	const double hShift = 0.0;
	const double scale = 1.0;
	const UnitizedEstimator<OutU, InU>& base;

public:
	AffineEstimator(
		const EstimatorId id,
		const UnitizedEstimator<OutU, InU>& base,
		const double vShift,
		const double hShift,
		const double scale
	) noexcept;

	Amount<OutU> get(const Amount<InU> input) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	AffineEstimator* clone() const override final;
};


template<Unit OutU, Unit InU>
AffineEstimator<OutU, InU>::AffineEstimator(
	const EstimatorId id,
	const UnitizedEstimator<OutU, InU>& base,
	const double vShift,
	const double hShift,
	const double scale
) noexcept :
	UnitizedEstimator<OutU, InU>(id, base.getMode()),
	vShift(vShift),
	hShift(hShift),
	scale(scale),
	base(base)
{}

template<Unit OutU, Unit InU>
Amount<OutU> AffineEstimator<OutU, InU>::get(const Amount<InU> input) const
{
	return (base.get(input - hShift) + vShift) * scale;
}

template<Unit OutU, Unit InU>
bool AffineEstimator<OutU, InU>::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	if (EstimatorBase::isEquivalent(other, epsilon) == false)
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return
		Utils::equal(this->vShift, oth.vShift, epsilon) &&
		Utils::equal(this->hShift, oth.hShift, epsilon) &&
		Utils::equal(this->scale, oth.scale, epsilon) &&
		this->base.isEquivalent(oth.base);
}

template<Unit OutU, Unit InU>
AffineEstimator<OutU, InU>* AffineEstimator<OutU, InU>::clone() const
{
	return new AffineEstimator(*this);
}
