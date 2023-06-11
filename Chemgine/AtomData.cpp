#include "AtomData.hpp"

AtomData::AtomData(
	const AtomIdType id,
	const std::string& symbol,
	const std::string& name,
	const double weight,
	const uint8_t valence) :
	id(id),
	symbol(symbol),
	name(name),
	weight(weight),
	valence(valence)
{}