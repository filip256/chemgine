#pragma once

#include "BaseComponentData.hpp"
#include "Symbol.hpp"

class AtomData : public BaseComponentData
{
private:
	const std::vector<uint8_t> valences;

public:
	const Symbol symbol;
	const std::string& name;

	AtomData(
		const AtomId id,
		const Symbol symbol,
		const std::string& name,
		const Amount<Unit::GRAM> weight,
		std::vector<uint8_t>&& valences) noexcept;

	AtomData(const AtomData&) = delete;
	AtomData(AtomData&&) = default;
	~AtomData() = default;

	bool isRadical() const;

	bool hasValence(const uint8_t v) const;
	const std::vector<uint8_t>& getValences() const;

	uint8_t getFittingValence(const uint8_t bonds) const override final;
	std::string getSMILES() const override final;
	std::string getBinaryId() const override final;

	static uint8_t getRarityOf(const Symbol symbol);

	static const uint8_t NullValence = static_cast<uint8_t>(-1);
	static const std::vector<uint8_t> RadicalAnyValence;
};
