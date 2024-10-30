#include "EstimatorSpecifier.hpp"

using namespace Def;

EstimatorSpecifier::EstimatorSpecifier(
	const Unit outUnit,
	std::vector<Unit>&& inUnits
) noexcept :
	outUnit(outUnit),
	inUnits(std::move(inUnits))
{}
