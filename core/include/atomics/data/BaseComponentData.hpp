#pragma once

#include "data/values/Amount.hpp"

#include <cstdint>
#include <string>
#include <vector>

class BaseComponentData
{
private:
	uint8_t rarity = 255;

public:
	const Amount<Unit::GRAM> weight = 0.0;

	BaseComponentData();
	BaseComponentData(
		const Amount<Unit::GRAM> weight,
		const uint8_t rarity = 255) noexcept;
	BaseComponentData(BaseComponentData&&) = default;
	virtual ~BaseComponentData() = default;

	virtual uint8_t getFittingValence(const uint8_t bonds) const = 0;
	virtual std::string getSMILES() const = 0;

	uint8_t getRarity() const;
};
