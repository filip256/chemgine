#include "Catalyst.hpp"

Catalyst::Catalyst(const Reactable& reactable, const Amount<Unit::MOLAR_PERCENT> idealAmount) noexcept :
	reactable(reactable),
	idealAmount(idealAmount)
{}

const MoleculeIdType Catalyst::getId() const
{
	return reactable.getId();
}

const MolecularStructure& Catalyst::getStructure() const
{
	return reactable.getStructure();
}

std::unordered_map<c_size, c_size> Catalyst::matchWith(const MolecularStructure& structure) const
{
	return reactable.matchWith(structure);
}

std::optional<Catalyst> Catalyst::get(
	const std::string& smiles,
	const Amount<Unit::MOLAR_PERCENT> idealAmount)
{
	const auto r = Reactable::get(smiles);
	if (r.has_value())
		return Catalyst(r.value(), idealAmount);

	return std::nullopt;
}