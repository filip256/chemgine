#pragma once

#include "UnitizedEstimator.hpp"

template<Unit OutU, Unit... InUs>
class ConstantEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:
	const Amount<OutU> constant;

public:
	ConstantEstimator(
		const EstimatorId id,
		const Amount<OutU> constant
	) noexcept;

	Amount<OutU> get(const Amount<InUs>...) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	ConstantEstimator* clone() const override final;
};

template<Unit OutU, Unit... InUs>
ConstantEstimator<OutU, InUs...>::ConstantEstimator(
	const EstimatorId id,
	const Amount<OutU> constant
) noexcept :
	UnitizedEstimator<OutU, InUs...>(id, EstimationMode::CONSTANT),
	constant(constant)
{}

template<Unit OutU, Unit... InUs>
Amount<OutU> ConstantEstimator<OutU, InUs...>::get(const Amount<InUs>...) const
{
	return constant;
}

template<Unit OutU, Unit... InUs>
bool ConstantEstimator<OutU, InUs...>::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	if (not EstimatorBase::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->constant.equals(oth.constant, epsilon);
}

template<Unit OutU, Unit... InUs>
ConstantEstimator<OutU, InUs...>* ConstantEstimator<OutU, InUs...>::clone() const
{
	return new ConstantEstimator(*this);
}
