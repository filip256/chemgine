#include "AtomData.hpp"
#include "Utils.hpp"

AtomData::AtomData(
	const ComponentIdType id,
	const std::string& symbol,
	const std::string& name,
	const Amount<Unit::GRAM> weight,
	std::vector<uint8_t>&& valences
) noexcept :
	BaseComponentData(id, symbol, name, weight, rarityOf(symbol)),
	valences(Utils::toSortedSetVector(std::move(valences)))
{}

bool AtomData::hasValence(const uint8_t v) const
{
	for (size_t i = 0; i < valences.size(); ++i)
		if (v == valences[i])
			return true;
	return false;
}

uint8_t AtomData::getFittingValence(const uint8_t bonds) const
{
	for (size_t i = 0; i < valences.size(); ++i)
		if (bonds <= valences[i])
			return valences[i];
	return AtomData::noneValence;
}

const std::vector<uint8_t>& AtomData::getValences() const
{
	return valences;
}

uint8_t AtomData::rarityOf(const std::string& symbol)
{
	if (symbol == "C" || symbol == "H")
		return 1;
	if (symbol == "O" || symbol == "N")
		return 3;
	if (symbol == "Cl" || symbol == "Br" || symbol == "I")
		return 5;
	if (symbol == "S" || symbol == "P")
		return 7;
	if (symbol == "B")
		return 9;
	if (symbol == "F")
		return 21;

	return 255;
}