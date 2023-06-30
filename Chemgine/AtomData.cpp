#include "AtomData.hpp"

AtomData::AtomData(
	const ComponentIdType id,
	const std::string& symbol,
	const std::string& name,
	const double weight,
	const uint8_t valence
) noexcept :
	BaseComponentData(id, symbol, name, weight, valence)
{}