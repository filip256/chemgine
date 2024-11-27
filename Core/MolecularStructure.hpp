#pragma once

#include "Bond.hpp"
#include "Atom.hpp"
#include "Parsers.hpp"
#include "Printers.hpp"

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <memory>

class TextBlock;

class MolecularStructure
{
private:
    uint16_t impliedHydrogenCount = 0;
    std::vector<std::unique_ptr<const Atom>> atoms;
    std::vector<std::vector<Bond>> bonds;

    void rPrint(
        TextBlock& buffer,
        const size_t x,
        const size_t y,
        const c_size c,
        std::vector<uint8_t>& visited,
        const bool printImpliedHydrogens) const;

    static void insertCycleHeads(
        std::string& smiles,
        const std::vector<size_t>& insertPositions,
        const std::map<c_size, uint8_t>& cycleHeads);

    std::string rToSMILES(
        c_size c, c_size prev,
        std::vector<size_t>& insertPositions,
        uint8_t& cycleCount,
        std::map<c_size, uint8_t>& cycleHeads,
        const size_t insertOffset) const;

    void removeAtom(const c_size idx);

    void removeUnnecessaryHydrogens();

    /// <summary>
    /// Returns the number of required hydrogens in order to complete the atom's valence.
    /// If the valences of the atom aren't respected it returns -1.
    /// Complexity: O(n_bonds)
    /// </summary>
    int8_t countImpliedHydrogens(const c_size idx) const;
    /// <summary>
    /// Returns the number of required hydrogens in order to complete the molecule.
    /// If the valences of the atoms aren't respected it returns -1.
    /// Complexity: O(n_comps * n_bonds)
    /// </summary>
    int16_t countImpliedHydrogens() const;

    // basic matching
    static bool areMatching(
        const c_size idxA, const MolecularStructure& a,
        const c_size idxB, const MolecularStructure& b,
        const bool escapeRadicalTypes);

    // finner matching for the compare method
    static bool areMatching(
        const Bond& nextA, const MolecularStructure& a,
        const Bond& nextB, const MolecularStructure& b,
        const bool escapeRadicalTypes);

    static uint8_t getBondSimilarity(
        const c_size idxA, const MolecularStructure& a,
        const c_size idxB, const MolecularStructure& b);

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
        c_size idxA, const MolecularStructure& a,
        c_size idxB, const MolecularStructure& b,
        std::vector<uint8_t>& visitedB,
        std::unordered_map<c_size, c_size>& mapping,
        bool escapeRadicalTypes
    );

    static std::pair<std::unordered_map<c_size, c_size>, uint8_t> DFSMaximal(
        c_size idxA, const MolecularStructure& a,
        std::unordered_set<c_size>& mappedA,
        c_size idxB, const MolecularStructure& b,
        std::unordered_set<c_size>& mappedB
    );

    /// <summary>
    /// Checks if the connectivity of pattern is preserved in the target.
    /// Complexity: O(n*m*b)
    /// </summary>
    static bool checkConnectivity(
        const MolecularStructure& target,
        const MolecularStructure& pattern,
        const std::unordered_map<c_size, c_size>& mapping);

    MolecularStructure(const MolecularStructure& other) noexcept;

public:
    static constexpr c_size npos = static_cast<c_size>(-1);

    MolecularStructure(const std::string& smiles);
    MolecularStructure(MolecularStructure&& structure) = default;
    ~MolecularStructure() noexcept;

    MolecularStructure& operator=(MolecularStructure&&) = default;

    /// <summary>
    /// Sorts atoms and bonds in decreasing order of atom precedence.
    /// Normalization simplifies algorithms and speeds up comparison.
    /// Complexity: O(n_comps * n_bonds * n_bonds)
    /// </summary>
    void canonicalize();

    const Atom& getAtom(const c_size idx) const;

    bool loadFromSMILES(const std::string& smiles);
    // not working for cycles :(
    std::string toSMILES() const;
    std::string print() const;

    /// <summary>
    /// Complexity: O(1)
    /// </summary>
    c_size getImpliedHydrogenCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// #Requires canonicalization
    /// </summary>
    c_size getRadicalAtomsCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    Amount<Unit::GRAM_PER_MOLE> getMolarMass() const;

    /// <summary>
    /// Complexity: O(1) (for now)
    /// </summary>
    uint8_t getDegreesOfFreedom() const;

    /// <summary>
    /// Checks if the molecule contains no radical atoms. 
    /// Complexity: O(1)
    /// #Requires canonicalization
    /// </summary>
    bool isConcrete() const;

    /// <summary>
    /// Checks if the molecule contains at least one radical atom. 
    /// Complexity: O(1)
    /// #Requires canonicalization
    /// </summary>
    bool isGeneric() const;

    /// <summary>
    /// Checks if the molecule contains a C-H bond. 
    /// Complexity: O(n)
    /// </summary>
    bool isOrganic() const;

    /// <summary>
    /// Returns a map representing a histrogram of all the atoms in this structure.
    /// Complexity: O(n)
    /// </summary>
    std::unordered_map<Symbol, c_size> getComponentCountMap() const;

    /// <summary>
    /// Returns true if the molecule contains no real or virtual atoms.
    /// Complexity: O(1)
    /// </summary>
    bool isEmpty() const;

    /// <summary>
    /// Virtual hydrogens not included.
    /// Complexity: O(1)
    /// </summary>
    inline c_size getNonVirtualAtomCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    c_size virtualBondCount() const;

    bool isCyclic() const;
    bool isConnected() const;

    /// <summary>
    /// Returns true if this is a pure virtual H2 molecule.
    /// </summary>
    bool isVirtualHydrogen() const;

    /// <summary>
    /// Checks if the two atoms are adjacent.
    /// Complexity: O(n)
    /// </summary>
    bool areAdjacent(const c_size idxA, const c_size idxB) const;

    void clear();

    /// <summary>
    /// Returns the number of bonds connected to a atoms (non-single bonds are taken into account)
    /// Complexity: O(n)
    /// </summary>
    /// <returns></returns>
    uint8_t getDegreeOf(const c_size idx) const;

    MolecularStructure createCopy() const;

    /// <summary>
    /// Returns the first found mapping between the atoms of the pattern and the atoms of *this.
    /// The whole pattern structure must be matched. 
    /// Complexity: rather large
    /// </summary>
    std::unordered_map<c_size, c_size> mapTo(const MolecularStructure& pattern, bool escapeRadicalTypes) const;

    /// <summary>
    /// Returns the largest mapping between the atoms of the pattern and the atoms of *this.
    /// Complexity: rather large
    /// </summary>
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> maximalMapTo(
        const MolecularStructure& pattern,
        const std::unordered_set<c_size>& targetIgnore = std::unordered_set<c_size>(),
        const std::unordered_set<c_size>& patternIgnore = std::unordered_set<c_size>()
    ) const;

    void recountImpliedHydrogens();

    /// <summary>
    /// Copies the branch starting at sourceIdx from source into the destination, using the mapping
    /// in order to avoid copying unwanted branches and resolve cycles.
    /// </summary>
    /// <param name="sourceIdx">: the common atom between the destination and source, where the branch starts</param>
    /// <param name="sdMapping">: a map between the atoms of the source and those of the destination.</param>
    /// <param name="canonicalize">: if true, canonicalization and implied hydrogen recount occurs after the copy is made and sdMapping is invalidated. </param>
    static void copyBranch(
        MolecularStructure& destination,
        const MolecularStructure& source,
        const c_size sourceIdx,
        std::unordered_map<c_size, c_size>& sdMapping,
        bool canonicalize = true,
        const std::unordered_set<c_size>& sourceIgnore = std::unordered_set<c_size>());

    /// <summary>
    /// Returns a molecule derived from pattern by adding all the substituents of instance that start from common atoms.
    /// </summary>
    /// <param name="pattern">: the base structure </param>
    /// <param name="instance">: a structure that has common substructures with the pattern </param>
    /// <param name="ipMap">: a map between the common atoms of pattern and instance </param>
    static MolecularStructure addSubstituents(
        const MolecularStructure& pattern,
        const MolecularStructure& instance,
        std::unordered_map<c_size, c_size>& ipMap,
        bool canonicalize = true);

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

    static std::optional<MolecularStructure> create(const std::string& smiles);
};


template <>
class Def::Parser<MolecularStructure>
{
public:
    static std::optional<MolecularStructure> parse(const std::string& str)
    {
        return MolecularStructure::create(str);
    }
};

template <>
class Def::Printer<MolecularStructure>
{
public:
    static std::string print(const MolecularStructure& object)
    {
        return object.toSMILES();
    }
};

