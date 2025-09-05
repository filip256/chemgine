#pragma once

#include "data/Accessor.hpp"
#include "molecules/data/GenericMoleculeData.hpp"

#include <optional>

class StructureRef : public Accessor<>
{
private:
    const GenericMoleculeData& data;

    StructureRef(const GenericMoleculeData& data) noexcept;

public:
    StructureRef(const StructureRef&) = default;

    MoleculeId                getId() const;
    const MolecularStructure& getStructure() const;

    std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const;
    std::unordered_map<c_size, c_size> matchWith(const StructureRef& other) const;
    bool                               matchesWith(const StructureRef& other) const;

    bool operator==(const StructureRef& other) const;
    bool operator!=(const StructureRef& other) const;

    static std::optional<StructureRef> create(MolecularStructure&& structure);
    static std::optional<StructureRef> create(const std::string& smiles);
};

template <>
struct std::hash<StructureRef>
{
    size_t operator()(const StructureRef& reactable) const { return std::hash<MoleculeId>()(reactable.getId()); }
};

template <>
struct std::hash<std::pair<StructureRef, uint8_t>>
{
    size_t operator()(const std::pair<StructureRef, uint8_t>& reactable) const
    {
        return std::hash<StructureRef>()(reactable.first);
    }
};
