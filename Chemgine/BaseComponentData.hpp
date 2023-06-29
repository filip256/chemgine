#pragma once

#include <cstdint>
#include <string>

typedef uint16_t ComponentIdType;

class BaseComponentData
{
public:
	const uint8_t valence;
	const ComponentIdType id;
	const std::string symbol, name;
	const double weight;

	BaseComponentData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		const double weight,
		const uint8_t valence);
};