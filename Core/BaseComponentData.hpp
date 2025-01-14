#pragma once

#include "Units.hpp"

#include <cstdint>
#include <string>
#include <vector>

class BaseComponentData
{
private:
	uint8_t rarity = 255;

public:
	const Quantity<Gram> weight = 0.0f * _Gram;

	BaseComponentData();
	BaseComponentData(
		const Quantity<Gram> weight,
		const uint8_t rarity = 255);
	BaseComponentData(BaseComponentData&&) = default;
	virtual ~BaseComponentData() = default;

	virtual uint8_t getFittingValence(const uint8_t bonds) const = 0;
	virtual std::string getSMILES() const = 0;

	uint8_t getRarity() const;
};
