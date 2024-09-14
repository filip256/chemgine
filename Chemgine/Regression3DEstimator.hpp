#pragma once

#include "UnitizedEstimator.hpp"
#include "Regressors3D.hpp"

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
class Regression3DEstimator : public UnitizedEstimator<OutU, InU1, InU2>
{
	static_assert(std::is_base_of_v<Regressor3DBase, RegT>,
		"Regression2DEstimator: RegT must be a Regressor3DBase derived type.");

private:
	const RegT regressor;

public:
	Regression3DEstimator(
		const EstimatorId id,
		const RegT& regressor,
		const EstimationMode mode
	) noexcept;

	Amount<OutU> get(const Amount<InU1> input1, const Amount<InU2> input2) const override final;

	bool isEquivalent(const EstimatorBase& other,
		const double epsilon = std::numeric_limits<double>::epsilon()
	) const override final;

	Regression3DEstimator* clone() const override final;
};

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
Regression3DEstimator<RegT, OutU, InU1, InU2>::Regression3DEstimator(
	const EstimatorId id,
	const RegT& regressor,
	const EstimationMode mode
) noexcept :
	UnitizedEstimator<OutU, InU1, InU2>(id, mode),
	regressor(regressor)
{}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
Amount<OutU> Regression3DEstimator<RegT, OutU, InU1, InU2>::get(const Amount<InU1> input1, const Amount<InU2> input2) const
{
	return regressor.get(input1.asStd(), input2.asStd());
}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
bool Regression3DEstimator<RegT, OutU, InU1, InU2>::isEquivalent(const EstimatorBase& other, const double epsilon) const
{
	if (not UnitizedEstimator<OutU, InU1, InU2>::isEquivalent(other, epsilon))
		return false;

	const auto& oth = static_cast<decltype(*this)&>(other);
	return this->regressor.isEquivalent(oth.regressor, epsilon);
}

template<typename RegT, Unit OutU, Unit InU1, Unit InU2>
Regression3DEstimator<RegT, OutU, InU1, InU2>* Regression3DEstimator<RegT, OutU, InU1, InU2>::clone() const
{
	return new Regression3DEstimator<RegT, OutU, InU1, InU2>(*this);
}
