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
    enum class Traits : uint8_t
    {
        NONE = 0,

        IS_HYDROGEN = 1 << 0,
        IS_GENERIC  = 1 << 1,
        IS_ORGANIC  = 1 << 2,

        REQUIRES_EXHAUSTIVE_MAPPING = 1 << 3
    };

private:
    std::vector<std::unique_ptr<BondedAtomBase>> atoms;
    Amount<Unit::GRAM_PER_MOLE>                  molarMass            = 0.0f;
    uint16_t                                     impliedHydrogenCount = 0;
    c_size                                       bondCount            = 0;
    FlagField<Traits>                            traits               = Traits::NONE;

    static void addBond(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType);
    static bool addBondChecked(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType);

    BondedAtomBase* addAtom(std::unique_ptr<BondedAtomBase>&& atom, BondedAtomBase* prev, const BondType bondType);
    void            removeAtom(const c_size idx);
    void            mutateAtom(const c_size idx, const AtomBase& newAtom);

    bool determineProperties();

    bool isFullyConnected() const;

    MolecularStructure() = default;
    MolecularStructure(const MolecularStructure& other) noexcept;

    MolecularStructure(
        std::vector<std::unique_ptr<BondedAtomBase>>&& atoms,
        const Amount<Unit::GRAM_PER_MOLE>              molarMass,
        const uint16_t                                 impliedHydrogenCount,
        const c_size                                   bondCount,
        const FlagField<Traits>                        traits) noexcept;

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
    void makeMolecularHydrogen();

    bool loadFromSMILES(const std::string& smiles);
    bool loadFromASCII(const std::string& ascii);
    bool loadFromMolBin(std::istream& is);

public:
    void canonicalize();

    const AtomBase&       getAtom(const c_size idx) const;
    const BondedAtomBase& getBondedAtom(const c_size idx) const;

    std::string printInfo() const;

    /// <summary>
    /// Returns the number of required hydrogens in order to complete the atom's valence.
    /// If the valences of the atom aren't respected it returns -1.
    /// </summary>
    static int8_t getImpliedHydrogenCount(const BondedAtomBase& atom);
    c_size        getImpliedHydrogenCount() const;
    c_size        getNonImpliedAtomCount() const;
    c_size        getRadicalAtomsCount() const;
    c_size        getTotalAtomCount() const;
    c_size        getBondCount() const;
    c_size        getCycleCount() const;

    Amount<Unit::GRAM_PER_MOLE> getMolarMass() const;

    uint8_t getDegreesOfFreedom() const;

    bool isEmpty() const;
    bool isConnected() const;
    bool isGeneric() const;
    bool isConcrete() const;
    bool isOrganic() const;
    bool isCyclic() const;
    bool isVirtualHydrogen() const;

    /// <summary>
    /// Returns a map representing a histogram of all the atoms in this structure.
    /// Complexity: O(n)
    /// </summary>
    std::unordered_map<Symbol, c_size> getComponentCountMap() const;

    /// <summary>
    /// Checks if the two atoms are adjacent.
    /// Complexity: O(n)
    /// </summary>
    bool areAdjacent(const c_size idxA, const c_size idxB) const;

    void clear();

    MolecularStructure createCopy() const;

private:
    template <bool Exact, bool AlwaysExhaustive = false>
    std::unordered_map<c_size, c_size> _mapTo(const MolecularStructure& pattern) const;

public:
    /// <summary>
    /// Returns the first found mapping between the atoms of the pattern and the atoms of *this.
    /// Radicals are escaped but the whole pattern structure must be matched.
    /// Complexity: rather large
    /// </summary>
    template <bool AlwaysExhaustive = false>
    std::unordered_map<c_size, c_size> mapTo(const MolecularStructure& pattern) const;

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
