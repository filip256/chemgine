#pragma once

#include "molecules/MolecularStructure.hpp"
#include "molecules/MoleculeId.hpp"

class GenericMoleculeData
{
public:
    const MoleculeId id;

protected:
    MolecularStructure structure;

public:
    GenericMoleculeData(const MoleculeId id, MolecularStructure&& structure) noexcept;

    GenericMoleculeData(const GenericMoleculeData&) = delete;
    GenericMoleculeData(GenericMoleculeData&&)      = default;
    ~GenericMoleculeData()                          = default;

    const MolecularStructure& getStructure() const;
};
