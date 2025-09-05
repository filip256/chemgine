#include "estimators/EstimatorSpecifier.hpp"

using namespace def;

EstimatorSpecifier::EstimatorSpecifier(const Unit outUnit, std::vector<Unit>&& inUnits) noexcept :
    outUnit(outUnit),
    inUnits(std::move(inUnits))
{}
