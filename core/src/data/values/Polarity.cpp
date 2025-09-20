#include "data/values/Polarity.hpp"

#include <cmath>

Polarity::Polarity(Amount<Unit::MOLE_RATIO> hydrophilicity, Amount<Unit::MOLE_RATIO> lipophilicity) noexcept :
    hydrophilicity(hydrophilicity),
    lipophilicity(lipophilicity)
{}

Amount<Unit::NONE> Polarity::getPartitionCoefficient() const
{
    const auto hydro = hydrophilicity == 0.0         ? Amount<Unit::MOLE_RATIO>::Epsilon
                       : hydrophilicity.isInfinity() ? Amount<Unit::MOLE_RATIO>::Maximum
                                                     : hydrophilicity;

    const auto lipo = lipophilicity == 0.0         ? Amount<Unit::MOLE_RATIO>::Epsilon
                      : lipophilicity.isInfinity() ? Amount<Unit::MOLE_RATIO>::Maximum
                                                   : lipophilicity;

    return std::log10((lipo / hydro).asStd());
}
