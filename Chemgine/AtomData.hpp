#pragma once

#include "Result.hpp"

#include <string>

typedef uint16_t AtomIdType;

class AtomData
{
public:
	const uint8_t valence;
	const AtomIdType id;
	const std::string symbol, name;
	const double weight;

	AtomData(
		const AtomIdType id,
		const std::string& symbol,
		const std::string& name,
		const double weight,
		const uint8_t valence);
};