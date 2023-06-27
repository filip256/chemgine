#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stack>
#include "Bond.hpp"

class MolecularStructure
{
private:
    uint16_t hydrogenCount = 0;
    std::vector<BaseComponent*> components;
    std::vector<std::vector<Bond>> bonds;

    void rPrint(
        std::vector<std::string>& buffer,
        const size_t x,
        const size_t y,
        const size_t c,
        std::vector<uint8_t>& visited) const;

    /// <summary>
    /// Returns the number of required hydrogens in order to complete the molecule.
    /// If the valences of the components aren't respected it returns -1.
    /// Complexity: O(n_comps * n_bonds)
    /// </summary>
    /// <returns></returns>
    int16_t getHCount() const;

public:
    MolecularStructure(const std::string& smiles);
    ~MolecularStructure();

    const BaseComponent* getComponent(const size_t idx) const;
    std::string print(const size_t maxWidth = 100, const size_t maxHeight = 50) const;
    bool loadFromSMILES(const std::string& smiles);

    /// <summary>
    /// Complexity: O(1)
    /// </summary>
    /// <returns></returns>
    uint16_t getHydrogenCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    /// <returns></returns>
    double getMolarMass() const;

    void clear();
};