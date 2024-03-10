#include "BaseComponentData.hpp"

BaseComponentData::BaseComponentData() {}

BaseComponentData::BaseComponentData(
	const ComponentId id,
	const Amount<Unit::GRAM> weight,
	const uint8_t rarity
) :
	id(id),
	weight(weight),
	rarity(rarity)
{}

uint8_t BaseComponentData::getRarity() const
{
	return rarity;
}