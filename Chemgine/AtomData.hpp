#pragma once

#include "BaseComponentData.hpp"
#include "Symbol.hpp"

class AtomData : public BaseComponentData
{
private:
	const Symbol symbol;
	const std::string& name;
	const std::vector<uint8_t> valences;

public:
	AtomData(
		const ComponentId id,
		const Symbol& symbol,
		const std::string& name,
		const Amount<Unit::GRAM> weight,
		std::vector<uint8_t>&& valences) noexcept;

	AtomData(const AtomData&) = delete;
	AtomData(AtomData&&) = default;
	~AtomData() = default;

	bool hasValence(const uint8_t v) const;
	const std::vector<uint8_t>& getValences() const;

	uint8_t getFittingValence(const uint8_t bonds) const override final;
	std::string getSMILES() const override final;
	std::string getBinaryId() const override final;

	static uint8_t getRarityOf(const Symbol& symbol);

	static const uint8_t nullValence = static_cast<uint8_t>(-1);
};