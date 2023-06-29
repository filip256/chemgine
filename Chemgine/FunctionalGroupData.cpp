#include "FunctionalGroupData.hpp"

FunctionalGroupData::FunctionalGroupData(
	const ComponentIdType id,
	const std::string& symbol,
	const std::string& name,
	const double weight,
	const uint8_t valence
) :
	BaseComponentData(id, symbol, name, weight, valence)
{}