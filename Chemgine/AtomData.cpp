#include "AtomData.hpp"

AtomData::AtomData(
	const ComponentIdType id,
	const std::string& symbol,
	const std::string& name,
	const double weight,
	const uint8_t valence
) noexcept :
	BaseComponentData(id, symbol, name, weight, valence, rarityOf(symbol))
{}

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