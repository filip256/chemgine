#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>

#include "Bond.hpp"
#include "BaseComponent.hpp"

class MolecularStructure
{
private:
    uint16_t hydrogenCount = 0;
    std::vector<const BaseComponent*> components;
    std::vector<std::vector<Bond*>> bonds;

    void rPrint(
        std::vector<std::string>& buffer,
        const size_t x,
        const size_t y,
        const size_t c,
        std::vector<uint8_t>& visited) const;

    /// <summary>
    /// Normalizes the structure by ordering components and bonds in decreasing order of
    /// component precedence.
    /// Normalization simplifies algorithms and speeds up comparison.
    /// Complexity: O(n_comps * n_bonds * n_bonds)
    /// </summary>
    void normalize();

    /// <summary>
    /// Returns the number of required hydrogens in order to complete the molecule.
    /// If the valences of the components aren't respected it returns -1.
    /// Complexity: O(n_comps * n_bonds)
    /// </summary>
    /// <returns></returns>
    int16_t getHCount() const;

    // basic matching
    static bool areMatching(
        const size_t idxA, const MolecularStructure& a,
        const size_t idxB, const MolecularStructure& b);

    // finner matching for the compare method
    static bool areMatching(
        const Bond& nextA, const MolecularStructure& a,
        const Bond& nextB, const MolecularStructure& b,
        const bool escapeRadicalTypes);

    static uint8_t getBondSimilarity(
        const size_t idxA, const MolecularStructure& a,
        const size_t idxB, const MolecularStructure& b);

    // returns a similarity score for grading maximal mappings, a score of 0 means no matching
    static uint8_t maximalSimilarity(
        const Bond& nextA, const MolecularStructure& a,
        const Bond& nextB, const MolecularStructure& b);

    /// <summary>
    /// Tries to find the pattern structure into the target starting from the given indexes.
    /// A cycle will match with a smaller cycle, connectivity of the mapping must be checked after this function is called
    /// If successful it returns true.
    /// Max rec. depth: sizae of the longest atom chain in pattern
    /// </summary>
    /// <param name="idxA">: starting index in target</param>
    /// <param name="a">: target</param>
    /// <param name="idxB">: starting index in pattern</param>
    /// <param name="b">: pattern</param>
    /// <param name="visitedB">: vector with the size of the pattern, initilized to false</param>
    /// <param name="mapping">: empty map that will store all matching nodes at the end of the execution</param>
    static bool DFSCompare(
        size_t idxA, const MolecularStructure& a,
        size_t idxB, const MolecularStructure& b,
        std::vector<uint8_t>& visitedB,
        std::unordered_map<size_t, size_t>& mapping,
        bool escapeRadicalTypes
    );

    static std::pair<std::unordered_map<size_t, size_t>, uint8_t> DFSMaximal(
        size_t idxA, const MolecularStructure& a,
        std::unordered_set<size_t>& mappedA,
        size_t idxB, const MolecularStructure& b,
        std::unordered_set<size_t>& mappedB
    );

    /// <summary>
    /// Checks if the connectivity of pattern is preserved in the target.
    /// Complexity: O(n*m*b)
    static bool checkConnectivity(
        const MolecularStructure& target,
        const MolecularStructure& pattern,
        const std::unordered_map<size_t, size_t>& mapping);

public:
    constexpr static size_t npos = static_cast<size_t>(-1);

    MolecularStructure(const std::string& smiles);
    MolecularStructure(MolecularStructure&& structure) = default;
    MolecularStructure(const MolecularStructure&) = delete;
    ~MolecularStructure() noexcept;

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
    uint16_t getRadicalAtomsCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    /// <returns></returns>
    double getMolarMass() const;

    /// <summary>
    /// Checks if the molecule contains at least one radical type. 
    /// Complexity: O(n)
    /// </summary>
    /// <returns></returns>
    bool isComplete() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    std::unordered_map<ComponentIdType, size_t> getComponentCountMap() const;

    /// <summary>
    /// Hydrogens not included.
    /// Complexity: O(1)
    /// </summary>
    /// <returns></returns>
    size_t componentCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    size_t bondCount() const;

    bool isCyclic() const;

    /// <summary>
    /// Checks if the two components are adjacent.
    /// Complexity: O(n)
    /// </summary>
    bool areAdjacent(const size_t idxA, const size_t idxB) const;

    void clear();

    /// <summary>
    /// Returns the number of bonds connected to a components (non-single bonds are taken into account)
    /// Complexity: O(n)
    /// </summary>
    /// <returns></returns>
    uint8_t getDegreeOf(const size_t idx) const;


    /// <summary>
    /// Returns the first found mapping between the atoms of the pattern and the atoms of *this.
    /// The whole pattern strcture must be matched. 
    /// Complexity: rather large
    /// </summary>
    /// <param name="pattern"></param>
    std::unordered_map<size_t, size_t> mapTo(const MolecularStructure& pattern, bool escapeRadicalTypes) const;

    std::unordered_map<size_t, size_t> maximalMapTo(const MolecularStructure& pattern) const;


    /// <summary>
    /// Returns true iff both structures represent the exact same molecule.
    /// Complexity: rather large
    /// </summary>
    bool operator==(const MolecularStructure& other) const;
    /// <summary>
    /// Returns true iff the structures represent different molecules.
    /// Complexity: rather large
    /// </summary>
    bool operator!=(const MolecularStructure& other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;
};