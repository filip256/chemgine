#pragma once

#include "data/values/Amount.hpp"

class Polarity
{
public:
    Amount<Unit::MOLE_RATIO> hydrophilicity;
    Amount<Unit::MOLE_RATIO> lipophilicity;

    Polarity(Amount<Unit::MOLE_RATIO> hydrophilicity, Amount<Unit::MOLE_RATIO> lipophilicity) noexcept;

    Polarity(const Polarity&) = default;

    Amount<Unit::NONE> getPartitionCoefficient() const;
};
