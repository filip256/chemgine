#pragma once

#include "BaseComponentData.hpp"
#include "PrintSettings.hpp"
#include "ImmutableSet.hpp"
#include "Symbol.hpp"

#include <iostream>

class AtomData : public BaseComponentData
{
private:
	const ImmutableSet<uint8_t> valences;

public:
	const Symbol symbol;
	const std::string name;

	AtomData(
		const Symbol& symbol,
		const std::string& name,
		const Amount<Unit::GRAM> weight,
		ImmutableSet<uint8_t>&& valences
	) noexcept;

	AtomData(const AtomData&) = delete;
	AtomData(AtomData&&) = default;
	~AtomData() = default;

	bool isRadical() const;

	bool hasValence(const uint8_t v) const;
	const ImmutableSet<uint8_t>& getValences() const;

	uint8_t getFittingValence(const uint8_t bonds) const override final;
	std::string getSMILES() const override final;

	virtual void dumpDefinition(std::ostream& out, const bool prettify) const;
	void print(std::ostream& out = std::cout) const;

	static uint8_t getRarityOf(const Symbol& symbol);

	static const uint8_t NullValence = static_cast<uint8_t>(-1);
	static const ImmutableSet<uint8_t> RadicalAnyValence;
};
