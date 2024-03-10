#include "AtomData.hpp"
#include "Utils.hpp"

AtomData::AtomData(
	const ComponentId id,
	const Symbol& symbol,
	const std::string& name,
	const Amount<Unit::GRAM> weight,
	std::vector<uint8_t>&& valences
) noexcept :
	BaseComponentData(id, weight, getRarityOf(symbol)),
	symbol(symbol),
	name(name),
	valences(Utils::toSortedSetVector(std::move(valences)))
{}

bool AtomData::hasValence(const uint8_t v) const
{
	for (size_t i = 0; i < valences.size(); ++i)
		if (v == valences[i])
			return true;
	return false;
}

const std::vector<uint8_t>& AtomData::getValences() const
{
	return valences;
}

uint8_t AtomData::getFittingValence(const uint8_t bonds) const
{
	for (size_t i = 0; i < valences.size(); ++i)
		if (bonds <= valences[i])
			return valences[i];
	return AtomData::nullValence;
}

std::string AtomData::getSMILES() const
{
	const char* s = symbol.get2ByteRepr();
	return s[1] != '\0' ? std::string({s[0], s[1]}) : std::string({ s[0] });
}

std::string AtomData::getBinaryId() const
{
	//return std::string({ static_cast<char>(id) });
	return std::to_string(id);
}


uint8_t AtomData::getRarityOf(const Symbol& symbol)
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