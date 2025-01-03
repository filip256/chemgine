#include "EstimatorSpecifier.hpp"

using namespace Def;

EstimatorSpecifier::EstimatorSpecifier(
	const UnitId outUnit,
	std::vector<UnitId>&& inUnits
) noexcept :
	outUnit(outUnit),
	inUnits(std::move(inUnits))
{}
