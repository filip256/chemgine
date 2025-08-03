#pragma once

#include "EstimatorBase.hpp"
#include "EstimatorSpecifier.hpp"
#include "Amount.hpp"

template<Unit OutU, Unit... InUs>
class UnitizedEstimator : public EstimatorBase
{
protected:
	static def::EstimatorSpecifier getUnitSpecifier();

public:
	using EstimatorBase::EstimatorBase;

	virtual Amount<OutU> get(const Amount<InUs>... inputs) const = 0;
};

template<Unit OutU, Unit... InUs>
def::EstimatorSpecifier UnitizedEstimator<OutU, InUs...>::getUnitSpecifier()
{
	return def::EstimatorSpecifier(OutU, { InUs... });
}


template<Unit OutU, Unit... InUs>
using EstimatorRef = CountedRef<const UnitizedEstimator<OutU, InUs...>>;
