#pragma once

#include "data/def/Parsers.hpp"
#include "data/def/Printers.hpp"
#include "molecules/ASCIIStructurePrinter.hpp"

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class MolecularStructure
{
private:
    uint16_t                                     impliedHydrogenCount = 0;
    std::vector<std::unique_ptr<BondedAtomBase>> atoms;

    static void addBond(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType);
    static bool addBondChecked(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType);

    BondedAtomBase* addAtom(const Symbol& symbol, BondedAtomBase* prev, const BondType bondType);
    void            removeAtom(const c_size idx);
    void            mutateAtom(const c_size idx, const Atom& newAtom);

    /// <summary>
    /// Returns the number of required hydrogens in order to complete the molecule.
    /// If the valences of the atoms aren't respected it returns -1.
    /// Complexity: O(n_comps * n_bonds)
    /// </summary>
    int16_t countImpliedHydrogens() const;

    bool isFullyConnected() const;

    MolecularStructure() = default;
    MolecularStructure(const MolecularStructure& other) noexcept;

public:
    static constexpr c_size npos = static_cast<c_size>(-1);

    MolecularStructure(const std::string& smiles);
    MolecularStructure(MolecularStructure&& structure) = default;

    MolecularStructure& operator=(MolecularStructure&&) = default;

    static std::optional<MolecularStructure> fromSMILES(const std::string& smiles);
    static std::optional<MolecularStructure> fromASCII(const std::string& ascii);
    static std::optional<MolecularStructure> fromMolBin(std::istream& is);
    static std::optional<MolecularStructure> loadMolBinFile(const std::string& path);

    std::string      toSMILES(const c_size startAtomIdx = 0) const;
    ColoredTextBlock toASCII(const ASCII::PrintOptions options = ASCII::PrintOptions::Default) const;
    void             toMolBin(std::ostream& os) const;
    void             toMolBinFile(const std::string& path) const;

private:
    bool loadFromSMILES(const std::string& smiles);
    bool loadFromASCII(const std::string& ascii);
    bool loadFromMolBin(std::istream& is);

public:
    /// <summary>
    /// Sorts atoms and bonds in decreasing order of atom precedence.
    /// Normalization simplifies algorithms and speeds up comparison.
    /// Complexity: O(n_comps * n_bonds * n_bonds)
    /// </summary>
    void canonicalize();

    const Atom&           getAtom(const c_size idx) const;
    const BondedAtomBase& getBondedAtom(const c_size idx) const;

    std::string printInfo() const;

    /// <summary>
    /// Returns the number of required hydrogens in order to complete the atom's valence.
    /// If the valences of the atom aren't respected it returns -1.
    /// </summary>
    static int8_t getImpliedHydrogenCount(const BondedAtomBase& atom);

    /// <summary>
    /// Complexity: O(1)
    /// </summary>
    c_size getImpliedHydrogenCount() const;

    /// <summary>
    /// Complexity: O(n)
    /// </summary>
    c_size getRadicalAtomsCount() const;

    /// <summary>
    /// Complexity: equal to getBondCount()
    /// </summary>
    c_size getCycleCount() const;

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
    /// </summary>
    bool isConcrete() const;

    /// <summary>
    /// Checks if the molecule contains at least one radical atom.
    /// Complexity: O(1)
    /// </summary>
    bool isGeneric() const;

    /// <summary>
    /// Checks if the molecule contains a C-H bond.
    /// Complexity: O(n)
    /// </summary>
    bool isOrganic() const;

    /// <summary>
    /// Returns a map representing a histogram of all the atoms in this structure.
    /// Complexity: O(n)
    /// </summary>
    std::unordered_map<Symbol, c_size> getComponentCountMap() const;

    /// <summary>
    /// Returns true if the molecule contains no real or virtual atoms.
    /// Complexity: O(1)
    /// </summary>
    bool isEmpty() const;

    /// <summary>
    /// Returns the number of non-implied (i.e. hydrogen) atoms.
    /// Complexity: O(1)
    /// </summary>
    c_size getNonImpliedAtomCount() const;

    /// <summary>
    /// Returns the total number of atoms in the molecule.
    /// Complexity: O(1)
    /// </summary>
    c_size getTotalAtomCount() const;

    /// <summary>
    /// Returns the number of non-implied bonds.
    /// Complexity: O(n)
    /// </summary>
    c_size getBondCount() const;

    /// <summary>
    /// Checks if the molecule contains at least one cycle.
    /// Complexity: equal to getBondCount()
    /// </summary>
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
        const MolecularStructure&         pattern,
        const std::unordered_set<c_size>& targetIgnore  = std::unordered_set<c_size>(),
        const std::unordered_set<c_size>& patternIgnore = std::unordered_set<c_size>()) const;

    void recountImpliedHydrogens();

    /// <summary>
    /// Copies the branch starting at sourceIdx from source into the destination, using the mapping
    /// in order to avoid copying unwanted branches and resolve cycles.
    /// </summary>
    /// <param name="sourceIdx">: the common atom between the destination and source, where the
    /// branch starts</param> <param name="sdMapping">: a map between the atoms of the source and
    /// those of the destination.</param> <param name="canonicalize">: if true, canonicalization and
    /// implied hydrogen recount occurs after the copy is made and sdMapping is invalidated.
    /// </param>
    static void copyBranch(
        MolecularStructure&                 destination,
        const MolecularStructure&           source,
        const c_size                        sourceIdx,
        std::unordered_map<c_size, c_size>& sdMapping,
        bool                                canonicalize = true,
        const std::unordered_set<c_size>& sourceIgnore   = std::unordered_set<c_size>());

    /// <summary>
    /// Returns a molecule derived from pattern by adding all the substituents of instance that
    /// start from common atoms.
    /// </summary>
    /// <param name="pattern">: the base structure </param>
    /// <param name="instance">: a structure that has common substructures with the pattern </param>
    /// <param name="ipMap">: a map between the common atoms of pattern and instance </param>
    static MolecularStructure addSubstituents(
        const MolecularStructure&                 pattern,
        const MolecularStructure&                 instance,
        const std::unordered_map<c_size, c_size>& ipMap);

    using Cycle = std::vector<const BondedAtomBase*>;
    /// <summary>
    /// Computes and returns the fundamental cycles set of the molecule.
    /// </summary>
    std::vector<Cycle> getFundamentalCycleBasis() const;
    /// <summary>
    /// Computes and returns the set of minimal non-overlapping cycles of the molecule.
    /// </summary>
    std::vector<Cycle> getMinimalCycleBasis() const;

    /// <summary>
    /// Returns true if both structures represent the exact same molecule.
    /// Complexity: rather large
    /// </summary>
    bool operator==(const MolecularStructure& other) const;
    /// <summary>
    /// Returns true if the structures represent different molecules.
    /// Complexity: rather large
    /// </summary>
    bool operator!=(const MolecularStructure& other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;
};

template <>
class def::Parser<MolecularStructure>
{
public:
    static std::optional<MolecularStructure> parse(const std::string& str)
    {
        return MolecularStructure::fromSMILES(str);
    }
};

template <>
class def::Printer<MolecularStructure>
{
public:
    static std::string print(const MolecularStructure& object) { return object.toSMILES(); }
};
