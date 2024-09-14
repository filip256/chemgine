#pragma once

#include "EstimatorBase.hpp"
#include "EstimationMode.hpp"
#include "EstimatorSpecifier.hpp"
#include "DataPoint.hpp"
#include "DefinitionObject.hpp"
#include "Amount.hpp"

template<Unit OutU, Unit... InUs>
class UnitizedEstimator : public EstimatorBase
{
protected:
	const EstimationMode mode;

public:
	UnitizedEstimator(
		const EstimatorId id, 
		const EstimationMode mode
	) noexcept;

	virtual Amount<OutU> get(const Amount<InUs>... inputs) const = 0;

	EstimationMode getMode() const;
};

template<Unit OutU, Unit... InUs>
UnitizedEstimator<OutU, InUs...>::UnitizedEstimator(
	const EstimatorId id,
	const EstimationMode mode
) noexcept :
	EstimatorBase(id),
	mode(mode)
{}

template<Unit OutU, Unit... InUs>
EstimationMode UnitizedEstimator<OutU, InUs...>::getMode() const
{
	return mode;
}
