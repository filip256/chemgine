#pragma once

#include "Amount.hpp"

#include <cstdint>
#include <string>
#include <vector>

typedef uint16_t ComponentIdType;

class BaseComponentData
{
private:
	uint8_t rarity = 255;

public:
	const ComponentIdType id = 0;
	const Amount<Unit::GRAM> weight = 0.0;
	const std::string symbol, name;

	BaseComponentData();

	BaseComponentData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		const Amount<Unit::GRAM> weight,
		const uint8_t rarity = 255);

	BaseComponentData(BaseComponentData&&) noexcept = default;

	virtual uint8_t getFittingValence(const uint8_t bonds) const = 0;

	uint8_t getRarity() const;
};