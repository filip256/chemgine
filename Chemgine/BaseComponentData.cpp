#include "BaseComponentData.hpp"

BaseComponentData::BaseComponentData() {}

BaseComponentData::BaseComponentData(
	const ComponentIdType id,
	const std::string& symbol,
	const std::string& name,
	const double weight,
	const uint8_t valence,
	const uint8_t rarity
) :
	id(id),
	symbol(symbol),
	name(name),
	weight(weight),
	valence(valence),
	rarity(rarity)
{}

uint8_t BaseComponentData::getRarity() const
{
	return rarity;
}