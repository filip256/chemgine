#pragma once

#include "BaseComponentData.hpp"

class AtomData : public BaseComponentData
{
private:
	const std::vector<uint8_t> valences;

public:
	AtomData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		const Amount<Unit::GRAM> weight,
		std::vector<uint8_t>&& valences) noexcept;

	AtomData(const AtomData&) = delete;
	AtomData(AtomData&&) = default;
	~AtomData() = default;

	bool hasValence(const uint8_t v) const;
	uint8_t getFittingValence(const uint8_t bonds) const override final;

	const std::vector<uint8_t>& getValences() const;

	static uint8_t rarityOf(const std::string& symbol);

	static const uint8_t noneValence = static_cast<uint8_t>(-1);
};