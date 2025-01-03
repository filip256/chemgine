#pragma once

#include "UnitizedEstimator.hpp"

template<typename BaseRefT, UnitType OutU, UnitType... InUs>
class DerivedEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
	using Base = UnitizedEstimator<OutU, InUs...>;

	const BaseRefT base;

public:
	DerivedEstimator(
		const EstimatorId id,
		const BaseRefT& base
	) noexcept;

	const BaseRefT& getBase() const;

	bool isEquivalent(const DerivedEstimator& other,
		const float_s epsilon = std::numeric_limits<float_s>::epsilon()) const;

	uint16_t getNestingDepth() const override final;
};

template<typename BaseRefT, UnitType OutU, UnitType... InUs>
DerivedEstimator<BaseRefT, OutU, InUs...>::DerivedEstimator(
	const EstimatorId id,
	const BaseRefT& base
) noexcept:
	Base(id),
	base(base)
{}

template<typename BaseRefT, UnitType OutU, UnitType... InUs>
const BaseRefT& DerivedEstimator<BaseRefT, OutU, InUs...>::getBase() const
{
	return base;
}

template<typename BaseRefT, UnitType OutU, UnitType... InUs>
bool DerivedEstimator<BaseRefT, OutU, InUs...>::isEquivalent(const DerivedEstimator& other, const float_s epsilon) const
{
	return this->base->isEquivalent(*other.base);
}

template<typename BaseRefT, UnitType OutU, UnitType... InUs>
uint16_t DerivedEstimator<BaseRefT, OutU, InUs...>::getNestingDepth() const
{
	return 1 + base->getNestingDepth();
}
