#include "atomics/data/BaseComponentData.hpp"

BaseComponentData::BaseComponentData() {}

BaseComponentData::BaseComponentData(
	const Amount<Unit::GRAM> weight,
	const uint8_t rarity
) :
	weight(weight),
	rarity(rarity)
{}

uint8_t BaseComponentData::getRarity() const
{
	return rarity;
}