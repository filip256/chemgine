#include "BaseComponentData.hpp"

BaseComponentData::BaseComponentData() {}

BaseComponentData::BaseComponentData(
	const Quantity<Gram> weight,
	const uint8_t rarity
) :
	weight(weight),
	rarity(rarity)
{}

uint8_t BaseComponentData::getRarity() const
{
	return rarity;
}