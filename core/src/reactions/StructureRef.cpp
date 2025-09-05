#include "reactions/StructureRef.hpp"

#include "data/DataStore.hpp"

StructureRef::StructureRef(const GenericMoleculeData& data) noexcept :
    data(data)
{}

MoleculeId StructureRef::getId() const { return data.id; }

const MolecularStructure& StructureRef::getStructure() const { return data.getStructure(); }

std::unordered_map<c_size, c_size> StructureRef::matchWith(const MolecularStructure& structure) const
{
    const auto& thisStructure = getStructure();
    const auto  map           = structure.mapTo(thisStructure, true);
    if (map.size() == thisStructure.getNonImpliedAtomCount())
        return map;

    return std::unordered_map<c_size, c_size>();
}

std::unordered_map<c_size, c_size> StructureRef::matchWith(const StructureRef& other) const
{
    return this->matchWith(other.getStructure());
}

bool StructureRef::matchesWith(const StructureRef& other) const { return this->matchWith(other).size(); }

bool StructureRef::operator==(const StructureRef& other) const { return this->getStructure() == other.getStructure(); }

bool StructureRef::operator!=(const StructureRef& other) const { return this->getStructure() != other.getStructure(); }

std::optional<StructureRef> StructureRef::create(MolecularStructure&& structure)
{
    return not structure.isEmpty()
               ? std::optional(StructureRef(dataAccessor.get().molecules.findOrAdd(std::move(structure))))
               : std::nullopt;
}

std::optional<StructureRef> StructureRef::create(const std::string& smiles)
{
    return create(MolecularStructure(smiles));
}
