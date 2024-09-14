#pragma once

#include "UnitizedEstimator.hpp"
#include "Regressors2D.hpp"

template<typename RegT, Unit OutU, Unit InU>
class Regression2DEstimator : public UnitizedEstimator<OutU, InU>
{
	static_assert(std::is_base_of_v<Regressor2DBase, RegT>,
		"Regression2DEstimator: RegT must be a Regressor2DBase derived type.");

private:
	const RegT regressor;

public:
	Regression2DEstimator(
		const EstimatorId id,
		const RegT& regressor,
		const EstimationMode mode
	) noexcept;

	Amount<OutU> get(const Amount<InU> input) const override final;

	const RegT& getRegressor() const;

	bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	Regression2DEstimator* clone() const override final;
};

template<typename RegT, Unit OutU, Unit InU>
Regression2DEstimator<RegT, OutU, InU>::Regression2DEstimator(
	const EstimatorId id,
	const RegT& regressor,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU>(id, mode),
	regressor(regressor)
{}

template<typename RegT, Unit OutU, Unit InU>
Amount<OutU> Regression2DEstimator<RegT, OutU, InU>::get(const Amount<InU> input) const
{
	return regressor.get(input.asStd());
}

template<typename RegT, Unit OutU, Unit InU>
const RegT& Regression2DEstimator<RegT, OutU, InU>::getRegressor() const
{
	return regressor;
}

template<typename RegT, Unit OutU, Unit InU>
bool Regression2DEstimator<RegT, OutU, InU>::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	if (not UnitizedEstimator<OutU, InU>::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template<typename RegT, Unit OutU, Unit InU>
Regression2DEstimator<RegT, OutU, InU>* Regression2DEstimator<RegT, OutU, InU>::clone() const
{
	return new Regression2DEstimator<RegT, OutU, InU>(*this);
}
