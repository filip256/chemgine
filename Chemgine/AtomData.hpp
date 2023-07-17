#pragma once

#include "BaseComponentData.hpp"

class AtomData : public BaseComponentData
{
public:
	AtomData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		const double weight,
		const uint8_t valence) noexcept;

	AtomData(const AtomData&) = delete;
	AtomData(AtomData&&) = default;
	~AtomData() = default;

	static uint8_t rarityOf(const std::string& symbol);
};