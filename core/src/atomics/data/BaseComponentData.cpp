#include "atomics/data/BaseComponentData.hpp"

BaseComponentData::BaseComponentData() {}

BaseComponentData::BaseComponentData(const Amount<Unit::GRAM> weight, const uint8_t rarity) noexcept
    :
    rarity(rarity),
    weight(weight)
{}

uint8_t BaseComponentData::getRarity() const { return rarity; }
