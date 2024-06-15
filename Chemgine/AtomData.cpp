#include "AtomData.hpp"
#include "Utils.hpp"

const ImmutableSet<uint8_t> AtomData::RadicalAnyValence = { NullValence };

AtomData::AtomData(
	const AtomId id,
	const Symbol symbol,
	const std::string& name,
	const Amount<Unit::GRAM> weight,
	ImmutableSet<uint8_t>&& valences
) noexcept :
	BaseComponentData(id, weight, getRarityOf(symbol)),
	symbol(symbol),
	name(name),
	valences(std::move(valences))
{}

bool AtomData::isRadical() const
{
	return weight == 0.0;
}

bool AtomData::hasValence(const uint8_t v) const
{
	for (size_t i = 0; i < valences.size(); ++i)
		if (v == valences[i])
			return true;
	return false;
}

const ImmutableSet<uint8_t>& AtomData::getValences() const
{
	return valences;
}

uint8_t AtomData::getFittingValence(const uint8_t bonds) const
{
	if (valences == RadicalAnyValence)
		return bonds != 0 ? bonds : 1;

	for (size_t i = 0; i < valences.size(); ++i)
		if (bonds <= valences[i])
			return valences[i];

	return AtomData::NullValence;
}

std::string AtomData::getSMILES() const
{
	return symbol.isSingleByte() ?
		symbol.getAsString() :
		'[' + symbol.getAsString() + ']';
}

std::string AtomData::getBinaryId() const
{
	//return std::string({ static_cast<char>(id) });
	return std::to_string(id);
}

uint8_t AtomData::getRarityOf(const Symbol symbol)
{
	static const std::unordered_map<Symbol, uint8_t> rarities {
		{ "C", 1 },
		{ "O", 2 },
		{ "N", 3 },
		{ "Cl", 4 },
		{ "Br", 4 },
		{ "I", 5 },
		{ "F", 6 },
		{ "S", 7 },
		{ "P", 8 },
		{ "B", 9 },
		{ "Al", 10 },
		{ "Se", 11 },
		{ "H", 100 },
	};

	const auto r = rarities.find(symbol);
	return r != rarities.end() ?
		r->second :
		255;
}
