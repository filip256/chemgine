#pragma once

#include "EstimatorBase.hpp"
#include "EstimatorSpecifier.hpp"
#include "Units.hpp"

template<UnitType OutU, UnitType... InUs>
class UnitizedEstimator : public EstimatorBase
{
protected:
	static Def::EstimatorSpecifier getUnitSpecifier();

public:
	using EstimatorBase::EstimatorBase;

	virtual Quantity<OutU> get(const Quantity<InUs>... inputs) const = 0;
};

template<UnitType OutU, UnitType... InUs>
Def::EstimatorSpecifier UnitizedEstimator<OutU, InUs...>::getUnitSpecifier()
{
	return Def::EstimatorSpecifier(UnitId::of<OutU>(), { UnitId::of<InUs>()...});
}


template<UnitType OutU, UnitType... InUs>
using EstimatorRef = CountedRef<const UnitizedEstimator<OutU, InUs...>>;
