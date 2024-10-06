#pragma once

#include "EstimatorBase.hpp"
#include "Amount.hpp"

template<Unit OutU, Unit... InUs>
class UnitizedEstimator : public EstimatorBase
{
protected:
	static std::string getUnitSpecifier();

public:
	using EstimatorBase::EstimatorBase;

	virtual Amount<OutU> get(const Amount<InUs>... inputs) const = 0;
};

template<Unit OutU, Unit... InUs>
std::string UnitizedEstimator<OutU, InUs...>::getUnitSpecifier()
{
	std::string specifier;
	if constexpr (sizeof...(InUs) == 1)
	{
		((specifier += Amount<InUs>::unitSymbol()), ...);
	}
	else
	{
		specifier += '(';
		((specifier += Amount<InUs>::unitSymbol() + ','), ...);
		specifier.back() = ')';
	}

	specifier += "->" + Amount<OutU>::unitSymbol();
	return specifier;
}

template<Unit OutU, Unit... InUs>
using EstimatorRef = CountedRef<const UnitizedEstimator<OutU, InUs...>>;
