#include "BaseComponentData.hpp"

BaseComponentData::BaseComponentData() {}

BaseComponentData::BaseComponentData(
	const ComponentIdType id,
	const std::string& symbol,
	const std::string& name,
	const Amount<Unit::GRAM> weight,
	const uint8_t rarity
) :
	id(id),
	symbol(symbol),
	name(name),
	weight(weight),
	rarity(rarity)
{}

uint8_t BaseComponentData::getRarity() const
{
	return rarity;
}