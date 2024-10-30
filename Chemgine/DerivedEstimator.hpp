#pragma once

#include "UnitizedEstimator.hpp"

template<Unit OutU, Unit... InUs>
class DerivedEstimator : public UnitizedEstimator<OutU, InUs...>
{
private:

public:
	using UnitizedEstimator<OutU, InUs...>::UnitizedEstimator;
};
