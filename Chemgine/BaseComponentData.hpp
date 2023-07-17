#pragma once

#include <cstdint>
#include <string>

typedef uint16_t ComponentIdType;

class BaseComponentData
{
private:
	uint8_t rarity = 255;

public:
	const uint8_t valence = 0;
	const ComponentIdType id = 0;
	const std::string symbol, name;
	const double weight = 0.0;

	BaseComponentData();

	BaseComponentData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		const double weight,
		const uint8_t valence,
		const uint8_t rarity = 255);

	BaseComponentData(BaseComponentData&&) noexcept = default;

	uint8_t getRarity() const;
};