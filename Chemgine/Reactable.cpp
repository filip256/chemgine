#include "Reactable.hpp"
#include "DataStore.hpp"

Reactable::Reactable(const MoleculeId id, const bool isGeneric) noexcept :
	id(id),
	isGeneric(isGeneric)
{
	dataAccessor.crashIfUninitialized();
}

const MoleculeId Reactable::getId() const
{
	return id;
}

const MolecularStructure& Reactable::getStructure() const
{
	if(isGeneric)
		return dataAccessor.get().genericMolecules.at(id).getStructure();

	return dataAccessor.get().molecules.at(id).getStructure();
}

std::unordered_map<c_size, c_size> Reactable::matchWith(const MolecularStructure& structure) const
{
	const auto& thisStructure = getStructure();
	const auto map = structure.mapTo(thisStructure, true);
	if (map.size() == thisStructure.componentCount())
		return map;

	return std::unordered_map<c_size, c_size>();
}

std::unordered_map<c_size, c_size> Reactable::matchWith(const Reactable& other) const
{
	return this->matchWith(other.getStructure());
}

bool Reactable::matchesWith(const Reactable& other) const
{
	return this->matchWith(other).size();
}

bool Reactable::operator==(const Reactable& other) const
{
	return this->getStructure() == other.getStructure();
}

bool Reactable::operator!=(const Reactable& other) const
{
	return this->getStructure() != other.getStructure();
}

std::optional<Reactable> Reactable::get(MolecularStructure&& structure)
{
	if (structure.isEmpty())
		return std::nullopt;

	if (structure.isConcrete())
		return Reactable(dataAccessor.get().molecules.findOrAdd(std::move(structure)), false);

	return Reactable(dataAccessor.get().genericMolecules.findOrAdd(std::move(structure)), true);
}

std::optional<Reactable> Reactable::get(const std::string& smiles)
{
	return get(MolecularStructure(smiles));
}
