#include "MolecularStructure.hpp"

#include "Parsers.hpp"
#include "TextBlock.hpp"
#include "StringTable.hpp"
#include "Log.hpp"

#include <array>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <queue>
#include <boost/dynamic_bitset.hpp>

MolecularStructure::MolecularStructure(const std::string& smiles)
{
    loadFromSMILES(smiles);
}

MolecularStructure::MolecularStructure(const MolecularStructure& other) noexcept :
    impliedHydrogenCount(other.impliedHydrogenCount)
{
    this->atoms.reserve(other.atoms.size());
    for (const auto& otherAtom : other.atoms)
        this->atoms.emplace_back(otherAtom->clone());

    // Bond other atom references must be updated
    for (auto& a : this->atoms)
        for (auto& b : a->bonds)
            b.setOther(*this->atoms[b.getOther().index]);
}

std::optional<MolecularStructure> MolecularStructure::create(const std::string& smiles)
{
    MolecularStructure temp(smiles);
    return temp.isEmpty() ? std::nullopt : std::optional(std::move(temp));
}

MolecularStructure MolecularStructure::createCopy() const
{
    return MolecularStructure(*this);
}

void MolecularStructure::clear()
{
    atoms.clear();
    impliedHydrogenCount = 0;
}

//
// Deserialize From SMILES
//

void MolecularStructure::addBond(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType)
{
    from.bonds.emplace_back(Bond(to, bondType));
    to.bonds.emplace_back(Bond(from, bondType));
}

bool MolecularStructure::addBondChecked(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType)
{
    if (from.isSame(to))
        return false;

    const auto bondExists = std::any_of(from.bonds.begin(), from.bonds.end(),
        [&to](const auto& b) { return b.getOther().isSame(to); });
    if (bondExists)
        return false;

    addBond(from, to, bondType);
    return true;
}

BondedAtomBase* MolecularStructure::addAtom(const Symbol& symbol, BondedAtomBase* prev, const BondType bondType)
{
    // Ignore implied atoms.
    static const auto hydrogen = Atom("H");
    if (symbol == hydrogen.getData().symbol && bondType == BondType::SINGLE)
        return prev;

    auto& inserted = *atoms.emplace_back(BondedAtomBase::create(symbol, atoms.size(), {}));
    if(prev)
        addBond(inserted, *prev, bondType);

    return &inserted;
}

void MolecularStructure::removeAtom(const c_size idx)
{
    auto& atom = *atoms[idx];

    // Remove all bonds leading to the removed atom
    for (auto& bond : atom.bonds)
        std::erase_if(bond.getOther().bonds, [&](const auto& b) { return b.getOther().isSame(atom); });

    atoms.erase(atoms.begin() + idx);
}

void MolecularStructure::canonicalize()
{
    if (atoms.size() == 0)
        return;

    if (atoms.size() == 1)
    {
        atoms.front()->index = 0;
        return;
    }

    std::sort(atoms.begin(), atoms.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs->getAtom().getPrecedence() > rhs->getAtom().getPrecedence();
        });

    for (c_size i = 0; i < atoms.size(); ++i)
        atoms[i]->index = i;

    // Sort bonds by the precedence of the other atom and valence
    for (auto& a : atoms)
    {
        std::sort(a->bonds.begin(), a->bonds.end(), [this](const auto& lhs, const auto& rhs)
            {
                const auto lhsPrecedence = lhs.getOther().getAtom().getPrecedence();
                const auto rhsPrecedence = rhs.getOther().getAtom().getPrecedence();

                return
                    lhsPrecedence > rhsPrecedence ? true :
                    lhsPrecedence < rhsPrecedence ? false :
                    lhs.getValence() > rhs.getValence();
            });
    }
}

// Returns the number of bonds connected to an atom (non-single bonds are taken into account)
uint8_t getDegreeOf(const BondedAtomBase& atom)
{
    uint8_t cnt = 0;
    for (const auto& b : atom.bonds)
        cnt += b.getValence();
    return cnt;
}

/// Returns the number of required hydrogens in order to complete the atom's valence.
/// If the valences of the atom aren't respected it returns -1.
int8_t getImpliedHydrogenCount(const BondedAtomBase& atom)
{
    const auto d = getDegreeOf(atom);
    const auto v = atom.getAtom().getData().getFittingValence(d);

    return v == AtomData::NullValence ? -1 : v - d;
}

int16_t MolecularStructure::countImpliedHydrogens() const
{
    int16_t hCount = 0;
    for (const auto& a : atoms)
    {
        const auto h = ::getImpliedHydrogenCount(*a);
        if (h == -1)
            return -1;

        hCount += h;
    }
    return hCount;
}

bool MolecularStructure::loadFromSMILES(const std::string& smiles)
{
    clear();

    if (smiles == "HH") // The only purely virtual molecule
    {
        if (not Atom::isDefined("H"))
            return false;

        impliedHydrogenCount = 2;
        return true;
    }

    std::unordered_map<uint8_t, c_size> rings;
    std::stack<c_size> branches;

    BondedAtomBase* prev = nullptr; // TODO: Could unroll the first loop so that is always prev != nullptr
    BondType bondType = BondType::SINGLE;
    for(size_t i = 0; i < smiles.size(); ++i)
    {
        // Basic atom
        if (isalpha(smiles[i]))
        {
            if (Symbol symbol(smiles[i]); Atom::isDefined(symbol))
                prev = addAtom(symbol, prev, bondType);
            else if (Symbol symbol(smiles.substr(i, 2)); Atom::isDefined(symbol))
            {
                prev = addAtom(symbol, prev, bondType);
                ++i;
            }
            else
            {
                Log(this).error("Atomic symbol '{0}' at index {1} is undefined.", smiles[i], i);
                clear();
                return false;
            }

            bondType = BondType::SINGLE;
            continue;
        }

        // Branch begin
        if (smiles[i] == '(')
        {
            if (not prev)
            {
                Log(this).error("Branch begin token: '(' precedes any atoms.");
                clear();
                return false;
            }

            branches.emplace(prev->index);
            continue;
        }

        // Branch end
        if (smiles[i] == ')')
        {
            if (branches.empty())
            {
                Log(this).error("Unpaired token: ')' found at index {0}.", i);
                clear();
                return false;
            }

            prev = atoms[branches.top()].get();
            branches.pop();
            continue;
        }
        
        // Bond type
        if (const auto bondT = Bond::fromSMILES(smiles[i]); bondT != BondType::NONE)
        {
            bondType = bondT;
            continue;
        }

        // Special atom
        if (smiles[i] == '[')
        {
            const size_t t = smiles.find(']', i + 1);
            if (t == std::string::npos)
            {
                Log(this).error("Unpaired '[' token found at index {0}.", i);
                clear();
                return false;
            }

            const Symbol& symbol(smiles.substr(i + 1, t - i - 1));
            if (not Atom::isDefined(symbol))
            {
                Log(this).error("Atomic symbol '{0}' at index {1} is undefined.", symbol.getString(), i);
                clear();
                return false;
            }
            
            prev = addAtom(symbol, prev, bondType);
            bondType = BondType::SINGLE;

            i = t;
            continue;
        }

        // Double-digit ring label
        if (smiles[i] == '%')
        {
            if(not prev)
            {
                Log(this).error("Ring label precedes any atoms.");
                clear();
                return false;
            }

            if (i + 2 >= smiles.size() || not isdigit(smiles[i + 1]) || not isdigit(smiles[i + 2]))
            {
                Log(this).error("Missing multi-digit ring label.");
                clear();
                return false;
            }

            // int8_t is enough for two digits
            const uint8_t label = (smiles[i + 1] - '0') * 10 + smiles[i + 2] - '0';
            i += 2;

            if (not rings.contains(label))
            {
                rings.emplace(label, atoms.size() - 1);
                continue;
            }

            if (prev == nullptr)
            {
                Log(this).error("Illegal ring-closing bond with label: '{0}'.", label);
                clear();
                return false;
            }

            if (not addBondChecked(*prev, *atoms[rings[label]], bondType))
            {
                Log(this).error("Illegal ring-closing bond with label: '{0}'.", label);
                clear();
                return false;
            }

            bondType = BondType::SINGLE;
            continue;
        }

        // Single digit ring label
        if (isdigit(smiles[i]))
        {
            if (not prev)
            {
                Log(this).error("Ring label precedes any atoms.");
                clear();
                return false;
            }

            const uint8_t label = smiles[i] - '0';
            if (not rings.contains(label))
            {
                rings.emplace(label, atoms.size() - 1);
                continue;
            }
            
            if (not addBondChecked(*prev, *atoms[rings[label]], bondType))
            {
                Log(this).error("Illegal ring-closing bond with label: '{0}'.", label);
                clear();
                return false;
            }

            bondType = BondType::SINGLE;
            continue;
        }

        Log(this).error("Unrecognized symbol '{0}' found at index {1}.", smiles[i], i);
        clear();
        return false;
    }

    if(branches.empty() == false)
    {
        Log(this).error("Unpaired '(' found.");
        clear();
        return false;
    }

    const auto hCount = countImpliedHydrogens();
    if(hCount == -1)
    {
        Log(this).error("Valence of an atom was exceeded in SMILES: '{0}'.", smiles);
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;

    canonicalize();

    return true;
}

//
// Property getters
//

const Atom& MolecularStructure::getAtom(const c_size idx) const
{
    return atoms[idx]->getAtom();
}

c_size MolecularStructure::getImpliedHydrogenCount() const
{
    return impliedHydrogenCount;
}

Amount<Unit::GRAM_PER_MOLE> MolecularStructure::getMolarMass() const
{
    Amount<Unit::GRAM> cnt = 0;
    for (c_size i = 0; i < atoms.size(); ++i)
        cnt += atoms[i]->getAtom().getData().weight;

    cnt += Atom("H").getData().weight * impliedHydrogenCount;
    return cnt.to<Unit::GRAM_PER_MOLE>(Amount<Unit::MOLE>(1.0));
}

uint8_t MolecularStructure::getDegreesOfFreedom() const
{
    if (atoms.size() == 1) // mono-atomics have 3
        return 3;

    if (atoms.size() == 2 || isVirtualHydrogen()) // di-atomics have 5
        return 5;

    // TODO: find degreesOfFreedom for other molecules (6 is true for CH4)
    return 6;
}

c_size MolecularStructure::getRadicalAtomsCount() const
{
    c_size cnt = 0;

    // Canonicalization ensures radical atoms are always stored at the end.
    c_size i = atoms.size();
    while (i-- > 0 && atoms[i]->getAtom().isRadical())
        ++cnt;

    return cnt;
}

c_size MolecularStructure::getCycleCount() const
{
    return static_cast<c_size>(static_cast<int32_t>(getBondCount()) - atoms.size() + 1);
}

bool MolecularStructure::isConcrete() const
{
    // Canonicalization ensures radical atoms are always stored at the end.
    return isVirtualHydrogen() || not atoms.back()->getAtom().isRadical();
}

bool MolecularStructure::isGeneric() const
{
    return not isConcrete();
}

bool MolecularStructure::isOrganic() const
{
    // A molecule is considered organic when it contains a C-H bond.
    const auto hasOrganicBond = [](const auto& a)
        {
            static const auto carbon = Atom("C");
            static const auto hydrogen = Atom("H");

            if (a->getAtom() != carbon)
                return false;

            if (::getImpliedHydrogenCount(*a) > 0)
                return true;

            return std::any_of(a->bonds.begin(), a->bonds.end(), [](const auto& b)
                { return b.getOther().getAtom() == hydrogen; });
        };

    return std::any_of(atoms.begin(), atoms.end(), hasOrganicBond);
}

std::unordered_map<Symbol, c_size> MolecularStructure::getComponentCountMap() const
{
    std::unordered_map<Symbol, c_size> result;
    for (const auto& a : atoms)
    {
        const auto& symbol = a->getAtom().getData().symbol;
        if (auto it = result.find(symbol); it != result.end())
            ++it->second;
        else
            result.emplace(symbol, 1);
    }
    
    if (impliedHydrogenCount == 0)
        return result;

    static const auto hydrogen = Atom("H");
    const auto& hSymbol = hydrogen.getData().symbol;

    if (auto it = result.find(hSymbol); it != result.end())
        it->second += impliedHydrogenCount;
    else
        result.emplace(hSymbol, impliedHydrogenCount);

    return result;
}

bool MolecularStructure::isEmpty() const
{
    return impliedHydrogenCount == 0 && atoms.empty();
}

c_size MolecularStructure::getNonImpliedAtomCount() const
{
    return atoms.size();
}

c_size MolecularStructure::getTotalAtomCount() const
{
    return atoms.size() + impliedHydrogenCount;
}

c_size MolecularStructure::getBondCount() const
{
    c_size cnt = 0;
    for (const auto& a : atoms)
        cnt += a->bonds.size();

    // Two bonds are stored for each actual bond in the molecule.
    return cnt / 2;
}

bool MolecularStructure::isCyclic() const
{
    // Molecules are connected graphs, so cycles can only appear if E > V-1.
    return getBondCount() > atoms.size() - 1;
}

bool MolecularStructure::isConnected() const
{
    if (atoms.size() == 0)
        return true;

    if (getBondCount() < atoms.size() - 1)
        return false;

    std::vector<uint8_t> visited(atoms.size(), false);
    std::stack<c_size> stack;

    // DFS to check if all nodes are reachable.
    c_size c = 0;
    while (true)
    {
        visited[c] = true;

        for(const auto& b : atoms[c]->bonds)
            if (not visited[b.getOther().index])
                stack.push(b.getOther().index);

        if (stack.empty())
            break;

        c = stack.top();
        stack.pop();
    }

    return std::any_of(visited.begin(), visited.end(), [](const auto& v) { return not v; });
}

bool MolecularStructure::isVirtualHydrogen() const
{
    return atoms.empty() && impliedHydrogenCount == 2;
}

//
// Matching
//

bool MolecularStructure::areAdjacent(const c_size idxA, const c_size idxB) const
{
    const auto& atomA = *atoms[idxA];
    return std::any_of(atomA.bonds.begin(), atomA.bonds.end(), [&](const auto& b) { return b.getOther().isSame(atomA); });
}

bool areMatching(
    const BondedAtomBase& a, const BondedAtomBase& b,
    const bool escapeRadicalTypes)
{
    if (a.bonds.size() != b.bonds.size())
        return false;

    return escapeRadicalTypes ?
        b.getAtom().matches(a.getAtom()) :
        b.getAtom().equals(a.getAtom());
}

bool areMatching(
    const Bond& nextA, const Bond& nextB,
    const bool escapeRadicalTypes)
{
    if (nextA.getType() != nextB.getType())
        return false;

    const auto& otherA = nextA.getOther();
    const auto& otherB = nextB.getOther();

    // Escape radical types
    if (escapeRadicalTypes == true && otherB.getAtom().isRadical())
        return otherB.getAtom().matches(otherA.getAtom());

    if (not areMatching(otherA, otherB, escapeRadicalTypes))
        return false;

    // Test to see if both have the same types of bonds
    std::array<int8_t, BondType::BOND_TYPE_COUNT> counts{ 0 };
    for (c_size i = 0; i < otherA.bonds.size(); ++i)
    {
        ++counts[otherA.bonds[i].getType()];
        --counts[otherB.bonds[i].getType()];
    }

    return std::none_of(counts.begin(), counts.end(), [](const auto& c) { return c != 0; });
}

/// <summary>
/// Tries to find the pattern structure into the target starting from the given indexes.
/// A cycle will match with a smaller cycle, connectivity of the mapping must be checked after this function is called
/// If successful it returns true.
/// Max rec. depth: size of the longest atom chain in pattern
/// </summary>
/// <param name="a">: starting atom in target</param>
/// <param name="b">: starting atom in pattern</param>
/// <param name="visitedB">: vector with the size of the pattern, initialized to false</param>
/// <param name="mapping">: empty map that will store all matching nodes at the end of the execution</param>
bool DFSCompare(
    const BondedAtomBase& a, const BondedAtomBase& b,
    std::vector<uint8_t>& visitedB,
    std::unordered_map<c_size, c_size>& mapping,
    bool escapeRadicalTypes)
{
    mapping.emplace(a.index, b.index);
    visitedB[b.index] = true;

    for (const auto& bondB : b.bonds)
    {
        if (visitedB[bondB.getOther().index])
            continue;

        auto matchFound = false;
        for (const auto& bondA : a.bonds)
        {
            if (mapping.contains(bondA.getOther().index) || 
                not areMatching(bondA, bondB, escapeRadicalTypes))
                continue;

            if (DFSCompare(bondA.getOther(), bondB.getOther(), visitedB, mapping, escapeRadicalTypes))
            {
                matchFound = true;
                break;
            }
            
            // Revert wrong branch
            visitedB[bondB.getOther().index] = false;
            mapping.erase(bondA.getOther().index);
        }

        if (matchFound == false)
            return false;
    }

    return true;
}

bool checkConnectivity(
    const MolecularStructure& target,
    const MolecularStructure& pattern,
    const std::unordered_map<c_size, c_size>& mapping)
{
    // TODO: This checks if all the edges in the mapping are valid but it's very inefficient and it likely hides a bug.
    for (auto const& m : mapping)
    {
        for (auto const& n : mapping)
            if (pattern.areAdjacent(m.second, n.second) && not target.areAdjacent(m.first, n.first))
                return false;
    }
    return true;
}

std::unordered_map<c_size, c_size> MolecularStructure::mapTo(const MolecularStructure& pattern, bool escapeRadicalTypes) const
{
    if (pattern.atoms.size() > this->atoms.size() || pattern.atoms.size() == 0)
        return std::unordered_map<c_size, c_size>();

    for (const auto& a : this->atoms)
    {
        // Should start with a non radical type from pattern.
        // Canonicalization assures that if such atom exists, it is the first.
        if (areMatching(*a, *pattern.atoms.front(), escapeRadicalTypes))
        {
            std::vector<uint8_t> visited(pattern.atoms.size(), false);
            std::unordered_map<c_size, c_size> mapping;

            if (DFSCompare(*a, *pattern.atoms.front(), visited, mapping, escapeRadicalTypes) == false)
                continue;

            if (not pattern.isCyclic())
                return mapping;

            // Re-verifying connectivity is necessary for cycles
            if(checkConnectivity(*this, pattern, mapping))
                return mapping;
        }
    }

    return std::unordered_map<c_size, c_size>();
}

bool MolecularStructure::operator==(const MolecularStructure& other) const
{
    if (this->atoms.size() != other.atoms.size() || this->impliedHydrogenCount != other.impliedHydrogenCount)
        return false;

    return this->mapTo(other, false).size() == this->atoms.size();
}

bool MolecularStructure::operator!=(const MolecularStructure& other) const
{
    if (this->atoms.size() != other.atoms.size() || this->impliedHydrogenCount != other.impliedHydrogenCount)
        return true;

    return this->mapTo(other, false).size() != this->atoms.size();
}

bool MolecularStructure::operator==(const std::string& other) const
{
    return *this == MolecularStructure(other);
}

bool MolecularStructure::operator!=(const std::string& other) const
{
    return *this != MolecularStructure(other);
}

//
// Maximal Mapping
//

uint8_t getBondSimilarity(const BondedAtomBase& a, const BondedAtomBase& b)
{
    uint8_t score = 255;
    std::array<int8_t, BondType::BOND_TYPE_COUNT> counts{ 0 };

    for (const auto& bondA : a.bonds)
        ++counts[bondA.getType()];
    for (const auto& bondB : b.bonds)
        --counts[bondB.getType()];

    const uint8_t scorePerBond = a.bonds.size() / 255;
    for (const auto& c : counts)
        score -= c * scorePerBond;

    return score;
}

uint8_t maximalSimilarity(const Bond& nextA, const Bond& nextB)
{
    if (nextA.getType() != nextB.getType())
        return 0;

    if (not nextB.getOther().getAtom().equals(nextA.getOther().getAtom()))
        return 0;

    return getBondSimilarity(nextA.getOther(), nextB.getOther());
}

std::pair<std::unordered_map<c_size, c_size>, uint8_t> DFSMaximal(
    const BondedAtomBase& a, std::unordered_set<c_size>& mappedA,
    const BondedAtomBase& b, std::unordered_set<c_size>& mappedB)
{
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> newMap;
    newMap.first.emplace(a.index, b.index);
    mappedA.emplace(a.index);
    mappedB.emplace(b.index);

    for (const auto& bondB : b.bonds)
    {
        if (mappedB.contains(bondB.getOther().index))
            continue;

        // Only the largest mapping is added into the final but states need to be copied
        std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMapping;
        std::unordered_set<c_size> maxMappedA;
        std::unordered_set<c_size> maxMappedB;
        for (const auto& bondA : a.bonds)
        {
            if (mappedA.contains(bondA.getOther().index))
                continue;

            const auto score = maximalSimilarity(bondA, bondB);
            if (score == 0)
                continue;

            // Reversing bad branches isn't possible here, so copies are needed
            auto mappedACopy = mappedA;
            auto mappedBCopy = mappedB;
            auto subMap = DFSMaximal(bondA.getOther(), mappedACopy, bondB.getOther(), mappedBCopy);

            if (subMap.first.size() > maxMapping.first.size() ||
                (subMap.first.size() == maxMapping.first.size() && score > maxMapping.second))
            {
                maxMapping = std::move(subMap);
                maxMappedA = std::move(mappedACopy);
                maxMappedB = std::move(mappedBCopy);
            }
        }

        newMap.first.merge(std::move(maxMapping.first));
        newMap.second = maxMapping.second;
        mappedA.merge(std::move(maxMappedA));
        mappedB.merge(std::move(maxMappedB));
    }

    return newMap;
}

std::pair<std::unordered_map<c_size, c_size>, uint8_t> MolecularStructure::maximalMapTo(
    const MolecularStructure& pattern,
    const std::unordered_set<c_size>& targetIgnore,
    const std::unordered_set<c_size>& patternIgnore) const
{
    if (pattern.atoms.size() == 0 || this->atoms.size() == 0)
        return std::pair<std::unordered_map<c_size, c_size>, uint8_t>();

    // Find matching atom in both target and pattern
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMapping;
    uint8_t maxScore = 0;
    for (c_size i = 0; i < this->atoms.size(); ++i)
    {
        if (targetIgnore.contains(i))
            continue;

        if (maxMapping.first.contains(i))
            continue;

        for (c_size j = 0; j < pattern.atoms.size(); ++j)
        {
            if (patternIgnore.contains(j))
                continue;

            if(pattern.atoms[j]->getAtom().equals(this->atoms[i]->getAtom()) == false)
                continue;

            const auto score = getBondSimilarity(*this->atoms[i], *pattern.atoms[j]);

            std::unordered_set<c_size> mappedA(targetIgnore), mappedB(patternIgnore);
            auto map = DFSMaximal(*this->atoms[i], mappedA, *pattern.atoms[j], mappedB);

            // Picks largest mapping, then best 2nd comp. score, then best 1st comp. score
            if (map.first.size() > maxMapping.first.size() ||
                (map.first.size() == maxMapping.first.size() && (
                    map.second > maxMapping.second ||
                    (map.second == maxMapping.second &&
                        score > maxScore))))
            {
                maxMapping = std::move(map);
                maxScore = score;
            }
        }
    }

    return maxMapping;
}

//
// Substitution
//

void MolecularStructure::recountImpliedHydrogens()
{
    if(!isVirtualHydrogen())
        impliedHydrogenCount = countImpliedHydrogens();
}

void MolecularStructure::mutateAtom(const c_size idx, const Atom& newAtom)
{
    auto& oldBondedAtom = *atoms[idx];
    auto newBondedAtom = oldBondedAtom.mutate(newAtom);

    // Replace the old bond references with the new atom reference.
    for (auto& bond : newBondedAtom->bonds)
    {
        auto& otherBonds = bond.getOther().bonds;
        auto& backBond = *std::find_if(otherBonds.begin(), otherBonds.end(),
            [&](const auto& b) { return b.getOther().isSame(oldBondedAtom); });
        backBond.setOther(*newBondedAtom);
    }

    atoms[idx] = std::move(newBondedAtom);
}

void MolecularStructure::copyBranch(
    MolecularStructure& destination,
    const MolecularStructure& source,
    const c_size sourceIdx,
    std::unordered_map<c_size, c_size>& sdMapping,
    bool canonicalize,
    const std::unordered_set<c_size>& sourceIgnore)
{
    const auto& sourceAtom = source.atoms[sourceIdx];

    // Overwrites first matching radical atom
    const auto dstFirstIdx = sdMapping[sourceIdx];
    if (destination.atoms[dstFirstIdx]->getAtom().isRadical())
        destination.mutateAtom(dstFirstIdx, sourceAtom->getAtom());

    std::stack<c_size> stack;
    for (c_size i = 0; i < sourceAtom->bonds.size(); ++i)
        if(sdMapping.contains(sourceAtom->bonds[i].getOther().index) == false &&
            sourceIgnore.contains(sourceAtom->bonds[i].getOther().index) == false)
            stack.push(sourceAtom->bonds[i].getOther().index);

    if (stack.empty())
        return;

    while (stack.size())
    {
        const auto c = stack.top();
        stack.pop();

        // Add current node
        sdMapping.emplace(c, destination.atoms.size());
        destination.atoms.emplace_back(BondedAtomBase::create(source.atoms[c]->getAtom(), destination.atoms.size(), {}));

        // Add bonds to existing nodes and queue non-existing nodes
        for (const auto& bond : source.atoms[c]->bonds)
        {
            if (const auto sdIt = sdMapping.find(bond.getOther().index); sdIt != sdMapping.end())
            {
                addBond(*destination.atoms.back(), *destination.atoms[sdIt->second], bond.getType());
                continue;
            }

            if (not sourceIgnore.contains(bond.getOther().index))
                stack.push(bond.getOther().index);
        }
    }

    if (canonicalize)
    {
        destination.canonicalize();
        destination.recountImpliedHydrogens();
        sdMapping.clear();
    }
}

MolecularStructure MolecularStructure::addSubstituents(
    const MolecularStructure& pattern,
    const MolecularStructure& instance,
    const std::unordered_map<c_size, c_size>& ipMap)
{
    MolecularStructure result(pattern);

    // Maintain the original mapping to ensure only the these atoms are substituted.
    auto sdMap = ipMap;
    for (auto const& p : ipMap)
        copyBranch(result, instance, p.first, sdMap, false);

    result.canonicalize();
    result.recountImpliedHydrogens();

    return result;
}

//
// CycleClosureSet
//

class CycleClosureSet
{
    class CycleClosure
    {
    public:
        const c_size idxA, idxB, tag;

        CycleClosure(
            const c_size idxA,
            const c_size idxB,
            const c_size tag
        ) noexcept;
        CycleClosure(const CycleClosure&) = default;
    };

    class ClosureComparator
    {
    public:
        bool operator()(const CycleClosure& lhs, const CycleClosure& rhs) const;
    };

private:
    std::set<CycleClosure, ClosureComparator> closures;

public:
    CycleClosureSet() = default;
    CycleClosureSet(const CycleClosureSet&) = delete;
    CycleClosureSet(CycleClosureSet&&) = default;

    bool add(const c_size idxA, const c_size idxB);

    c_size size() const;
    bool contains(const c_size idxA, const c_size idxB) const;

    using Iterator = decltype(closures)::const_iterator;
    Iterator begin() const;
    Iterator end() const;
};

CycleClosureSet::CycleClosure::CycleClosure(
    const c_size idxA,
    const c_size idxB,
    const c_size tag
) noexcept :
    idxA(idxA),
    idxB(idxB),
    tag(tag)
{}

bool CycleClosureSet::ClosureComparator::operator()(const CycleClosure& lhs, const CycleClosure& rhs) const
{
    // Order the indexes because (x, y) should be equivalent to (y, x).
    const auto [lhsMin, lhsMax] = lhs.idxA < lhs.idxB ?
        std::pair(lhs.idxA, lhs.idxB) :
        std::pair(lhs.idxB, lhs.idxA);
    const auto [rhsMin, rhsMax] = rhs.idxA < rhs.idxB ?
        std::pair(rhs.idxA, rhs.idxB) :
        std::pair(rhs.idxB, rhs.idxA);

    // Sort in decreasing order of the first index, then second index.
    return
        lhsMin > rhsMin ? true :
        lhsMin < rhsMin ? false :
        lhsMax > rhsMax;
}

bool CycleClosureSet::add(const c_size idxA, const c_size idxB)
{
    return closures.emplace(idxA, idxB, closures.size() + 1).second;
}

c_size CycleClosureSet::size() const
{
    return static_cast<c_size>(closures.size());
}

bool CycleClosureSet::contains(const c_size idxA, const c_size idxB) const
{
    return closures.contains(CycleClosure(idxA, idxB, 0));
}

CycleClosureSet::Iterator CycleClosureSet::begin() const
{
    return closures.begin();
}

CycleClosureSet::Iterator CycleClosureSet::end() const
{
    return closures.end();
}

//
// Serialize to SMILES
//

std::string getCycleTagString(const c_size tag)
{
    return tag < 10 ?
        std::to_string(tag) :
        '%' + std::to_string(tag);
}

void rToSMILES(
    const BondedAtomBase* current, const BondedAtomBase* prev,
    std::vector<size_t>& insertPositions, CycleClosureSet& cycleClosures,
    std::string& smiles
);

void inline rNextToSMILES(
    const BondedAtomBase* current, const Bond& bondToNext,
    CycleClosureSet& cycleClosures, std::vector<size_t>& insertPositions,
    std::string& smiles)
{
    const auto next = &bondToNext.getOther();
    if (insertPositions[next->index] != std::string::npos)
    {
        if (cycleClosures.add(insertPositions[next->index], insertPositions[current->index]))
            smiles += bondToNext.getSMILES() + getCycleTagString(cycleClosures.size());
        return;
    }

    smiles += '(' + bondToNext.getSMILES();
    rToSMILES(next, current, insertPositions, cycleClosures, smiles);
    smiles += ')';
}

bool inline lastToSMILES(
    const BondedAtomBase* current, const Bond& bondToNext,
    CycleClosureSet& cycleClosures, std::vector<size_t>& insertPositions,
    std::string& smiles)
{
    const auto next = &bondToNext.getOther();
    if (insertPositions[next->index] != std::string::npos)
    {
        if (cycleClosures.add(insertPositions[next->index], insertPositions[current->index]))
            smiles += bondToNext.getSMILES() + getCycleTagString(cycleClosures.size());
        return false;
    }

    smiles += bondToNext.getSMILES();
    return true;
}

void rToSMILES(
    const BondedAtomBase* current, const BondedAtomBase* prev,
    std::vector<size_t>& insertPositions, CycleClosureSet& cycleClosures,
    std::string& smiles)
{
    while (true)
    {
        smiles += current->getAtom().getSMILES();
        insertPositions[current->index] = smiles.size();

        const auto neighbourCount = current->bonds.size();

        // ...-P-C
        //       ^
        if (neighbourCount == 1)
            return;

        // ...-P-C-N-...
        //       ^
        if (neighbourCount == 2)
        {
            const auto& bondToNext = &current->bonds.front().getOther() == prev ?
                current->bonds.back() :
                current->bonds.front();

            if (not lastToSMILES(current, bondToNext, cycleClosures, insertPositions, smiles))
                return;

            // Advance to the next atom instead of making a new recursive call.
            prev = current;
            current = &bondToNext.getOther();
            continue;
        }

        //   ...-A A-...
        //       |/
        // ...-P-C-N-...
        //       ^
        for (c_size i = 0; i < neighbourCount - 2; ++i)
        {
            const auto& bondToNext = current->bonds[i];
            if (&bondToNext.getOther() != prev)
                rNextToSMILES(current, bondToNext, cycleClosures, insertPositions, smiles);
        }

        //   ...-N N-...
        //       |/
        // ...-P-C-P-...
        //       ^

        // The last neighbour has to be printed without '()' because it's the main branch.
        // Since the last neighbour could be the previous atom, the second-to-last neighbour must also be checked.
        const auto& secondToLastBond = current->bonds[neighbourCount - 2];
        const auto& lastBond = current->bonds.back();

        if (&lastBond.getOther() != prev)
        {
            if (&secondToLastBond.getOther() != prev)
                rNextToSMILES(current, secondToLastBond, cycleClosures, insertPositions, smiles);

            if (not lastToSMILES(current, lastBond, cycleClosures, insertPositions, smiles))
                return;

            // Advance to the next atom instead of making a new recursive call.
            prev = current;
            current = &lastBond.getOther();
            continue;
        }

        if (not lastToSMILES(current, secondToLastBond, cycleClosures, insertPositions, smiles))
            return;

        // Advance to the next atom instead of making a new recursive call.
        prev = current;
        current = &secondToLastBond.getOther();
    }
}

std::string MolecularStructure::toSMILES() const
{
    if (isVirtualHydrogen())
        return "HH";

    if (atoms.empty())
        return "";

    std::string smiles;
    smiles.reserve(atoms.size());

    const auto current = atoms.front().get();
    smiles += current->getAtom().getSMILES();

    const auto neighbourCount = current->bonds.size();
    if (neighbourCount == 0)
        return smiles;

    std::vector<size_t> insertPositions(atoms.size(), std::string::npos);
    insertPositions.front() = smiles.size();
    CycleClosureSet cycleClosures;

    if (neighbourCount == 1)
    {
        smiles += current->bonds.front().getSMILES();
        rToSMILES(&current->bonds.front().getOther(), current, insertPositions, cycleClosures, smiles);
    }
    else
    {
        for (c_size i = 0; i < neighbourCount - 1; ++i)
            rNextToSMILES(current, current->bonds[i], cycleClosures, insertPositions, smiles);

        const auto& lastBond = current->bonds.back();
        const auto last = &lastBond.getOther();

        if (insertPositions[last->index] == std::string::npos)
        {
            // Since current is the first atom we dont' have to add a new cycle closure, we already know this isn't a new cycle.
            smiles += current->bonds.back().getSMILES();
            rToSMILES(&current->bonds.back().getOther(), current, insertPositions, cycleClosures, smiles);
        }
    }

    // Add cycle closures.
    for (const auto& closure : cycleClosures)
        smiles.insert(closure.idxA, getCycleTagString(closure.tag));

    if(smiles.back() != ')')
        return smiles;

    // Strip unnecessary parenthesis resulted from cycles, aka trailing ')' and their paired '('.
    // TODO: This is a bit inefficient, smiles could be generated without these parenthesis.
    auto idx = smiles.size() - 1;
    while (idx-- > 0 && smiles[idx] == ')');
    const auto lastIdx = idx; // Last non-')' symbol.

    std::vector<size_t> removeIdx;
    std::stack<size_t> parenthesisStack;
    while (idx-- > 0)
    {
        if (smiles[idx] == ')')
            parenthesisStack.push(idx);
        else if (smiles[idx] == '(')
        {
            if (parenthesisStack.empty())
                removeIdx.emplace_back(idx); // These are naturally sorted in reversed order.
            else
                parenthesisStack.pop();
        }
    }

    std::string strippedSmiles;
    strippedSmiles.reserve(smiles.size() - removeIdx.size() * 2);

    for (size_t i = 0; i <= lastIdx; ++i)
    {
        if (removeIdx.size() && i == removeIdx.back())
            removeIdx.pop_back();
        else
            strippedSmiles.push_back(smiles[i]);
    }

    return strippedSmiles;
}

//
// Structure Print
//

class Edge
{
public:
    const c_size idxA, idxB;

    Edge(
        const c_size idxA,
        const c_size idxB
    ) noexcept :
        idxA(std::min(idxA, idxB)),
        idxB(std::max(idxA, idxB))
    {}
    Edge(const Edge&) = default;

    bool operator==(const Edge& other) const
    {
        return this->idxA == other.idxA && this->idxB == other.idxB;
    }
};

template<>
struct std::hash<Edge>
{
    size_t operator() (const Edge& edge) const
    {
        static_assert(sizeof(c_size) * 2 <= sizeof(size_t), "Type 'c_size' is too large for perfect Edge hashing.");
        return (static_cast<size_t>(edge.idxA) << sizeof(c_size)) | edge.idxB;
    }
};

std::vector<std::vector<c_size>> MolecularStructure::getFundamentalCycleBasis() const
{
    // The cycle count is easy to compute and can be used to stop the algorithm early.
    const auto cycleCount = getCycleCount();
    if (cycleCount == 0)
        return std::vector<std::vector<c_size>>();

    // Paton's Algorithm
    std::vector<c_size> parents(atoms.size(), npos);
    constexpr auto rootParent = npos - 1; // Used to differentiate from non-visited.
    parents.front() = rootParent;

    std::stack<c_size> stack;
    c_size c = 0;

    for (const auto& b : atoms.front()->bonds)
    {
        const auto nextIdx = b.getOther().index;
        stack.push(nextIdx);
        parents[nextIdx] = 0; // The parent is the root.
    }

    std::vector<std::vector<c_size>> cycles;
    CycleClosureSet cycleClosures;

    // DFS to find the cycle closures.
    while (stack.size())
    {
        c = stack.top();
        stack.pop();

        for (const auto& b : atoms[c]->bonds)
        {
            const auto nextIdx = b.getOther().index;

            // Prev node
            if (parents[c] == nextIdx)
                continue;

            // New node
            if (parents[nextIdx] == npos)
            {
                stack.push(nextIdx);
                parents[nextIdx] = c;
                continue;
            }

            // Older cycle
            if (cycleClosures.contains(c, nextIdx))
                continue;

            // New cycle
            std::vector<c_size> cycle;

            // Since we use DFS to traverse we can just intersect root->...->parents[C]->C with parents[N]->N.
            // The intersection combined with C->N yields the cycle.
            for (c_size i = parents[c]; i != parents[nextIdx]; i = parents[i])
                cycle.emplace_back(i);

            cycle.emplace_back(parents[nextIdx]);
            cycle.emplace_back(nextIdx);
            cycle.emplace_back(c);
            cycles.emplace_back(std::move(cycle));
            if (cycles.size() == cycleCount)
                return cycles;

            cycleClosures.add(c, nextIdx);
        }
    }

    return cycles;
}

void MolecularStructure::getMinimalCycleBasis() const
{
    auto fundamentalCycles = getFundamentalCycleBasis();
    std::sort(fundamentalCycles.begin(), fundamentalCycles.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.size() < rhs.size();
        });

    // Assign an index to each unique edge.
    std::unordered_map<Edge, c_size> edgeLabels;
    std::vector<Edge> uniqueEdges;
    // Skip some initial reallocations, smallest number of edges in a cycle is 3.
    edgeLabels.reserve(fundamentalCycles.size() * 3);
    uniqueEdges.reserve(fundamentalCycles.size() * 3);

    static constexpr size_t MAX_CYCLIC_EDGE_COUNT = 64;
    std::vector<std::bitset<MAX_CYCLIC_EDGE_COUNT>> basis;

    for (const auto& cycle : fundamentalCycles)
    {
        std::bitset<MAX_CYCLIC_EDGE_COUNT> cycleEdges;
        boost::dynamic_bitset<uint8_t> B4(16, 84);
        std::cout << B4;

        const Edge edge(cycle.front(), cycle.back());
        if (const auto edgeIt = edgeLabels.find(edge); edgeIt != edgeLabels.end())
            cycleEdges.set(edgeIt->second);
        else
        {
            const auto newLabel = static_cast<c_size>(edgeLabels.size());
            edgeLabels.emplace(edge, newLabel);
            uniqueEdges.emplace_back(edge);
            cycleEdges.set(newLabel);
        }

        for (size_t i = 0; i < cycle.size() - 1; ++i)
        {
            const Edge edge(cycle[i], cycle[i + 1]);
            if (const auto edgeIt = edgeLabels.find(edge); edgeIt != edgeLabels.end())
            {
                cycleEdges.set(edgeIt->second);
                continue;
            }

            const auto newLabel = static_cast<c_size>(edgeLabels.size());
            edgeLabels.emplace(edge, newLabel);
            uniqueEdges.emplace_back(edge);
            cycleEdges.set(newLabel);
        }

        for (const auto& b : basis)
        {
            const auto temp = cycleEdges ^ b;
            if (temp.count() < cycleEdges.count())
                cycleEdges = temp;

            if (cycleEdges.none())
                break;
        }

        if (cycleEdges.none())
            continue;

        basis.emplace_back(cycleEdges);
    }

    for (const auto& c : basis)
    {
        for (size_t i = 0; i < c.size(); ++i)
        {
            if (c.test(i))
            {
                const auto& edge = uniqueEdges[i];
                std::cout << edge.idxA << '-' << edge.idxB << ' ';
            }
        }
        std::cout << '\n';
    }
}


void rPrint(
    TextBlock& buffer,
    const size_t x,
    const size_t y,
    const BondedAtomBase& current,
    std::vector<uint8_t>& visited,
    const bool printImpliedHydrogens)
{
    if (x < 1 || y < 1 || buffer[y][x] != ' ')
        return;

    visited[current.index] = true;

    const auto& symbol = current.getAtom().getSymbolStr();
    const uint8_t symbolSize = symbol.size();

    for (uint8_t i = 0; i < symbolSize; ++i)
        buffer[y][x + i] = symbol[i];

    for (const auto& b : current.bonds)
    {
        const auto& other = b.getOther();
        if (not visited[other.index])
        {
            char vC, hC, d1C, d2C;
            switch (b.getType())
            {
            case BondType::SINGLE:
                vC = '³', hC = 'Ä', d1C = '\\', d2C = '/';
                break;
            case BondType::DOUBLE:
                vC = 'º', hC = 'Í', d1C = hC, d2C = hC;
                break;
            case BondType::TRIPLE:
                vC = 'ð', hC = 'ð', d1C = hC, d2C = hC;
                break;
            case BondType::QUADRUPLE:
                vC = hC = d1C = d2C = '$';
                break;
            default:
                vC = hC = d1C = d2C = '?';
                break;
            }

            const uint8_t nextSymbolSize = other.getAtom().getSymbolStr().size();

            if (buffer[y][x + symbolSize + 1] == ' ')
            {
                buffer[y][x + symbolSize] = hC;
                rPrint(buffer, x + symbolSize + 1, y, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y][x - nextSymbolSize - 1] == ' ')
            {
                buffer[y][x - 1] = hC;
                rPrint(buffer, x - nextSymbolSize - 1, y, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x] == ' ')
            {
                buffer[y - 1][x] = vC;
                rPrint(buffer, x, y - 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x] == ' ')
            {
                buffer[y + 1][x] = vC;
                rPrint(buffer, x, y + 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x + symbolSize + 1] == ' ')
            {
                buffer[y + 1][x + symbolSize] = d1C;
                rPrint(buffer, x + symbolSize + 1, y + 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x - nextSymbolSize - 1] == ' ')
            {
                buffer[y + 1][x - 1] = d2C;
                rPrint(buffer, x - nextSymbolSize - 1, y + 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x + symbolSize + 1] == ' ')
            {
                buffer[y - 1][x + symbolSize] = d2C;
                rPrint(buffer, x + symbolSize + 1, y - 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x - nextSymbolSize - 1] == ' ')
            {
                buffer[y - 1][x - 1] = d1C;
                rPrint(buffer, x - nextSymbolSize - 1, y - 2, other, visited, printImpliedHydrogens);
            }
            else
            {
                Log<MolecularStructure>().warn("Incomplete ASCII print.");
                break;
            }
        }
    }

    if (printImpliedHydrogens)
    {
        const auto hCount = ::getImpliedHydrogenCount(current);
        for (int8_t i = 0; i < hCount; ++i)
        {
            const char vC = '³', hC = 'Ä', d1C = '\\', d2C = '/';

            if (buffer[y][x + symbolSize + 1] == ' ')
            {
                buffer[y][x + symbolSize] = hC;
                buffer[y][x + symbolSize + 1] = 'H';
            }
            else if (buffer[y][x - 2] == ' ')
            {
                buffer[y][x - 1] = hC;
                buffer[y][x - 2] = 'H';
            }
            else if (buffer[y - 2][x] == ' ')
            {
                buffer[y - 1][x] = vC;
                buffer[y - 2][x] = 'H';
            }
            else if (buffer[y + 2][x] == ' ')
            {
                buffer[y + 1][x] = vC;
                buffer[y + 2][x] = 'H';
            }
            else if (buffer[y + 2][x + symbolSize + 1] == ' ')
            {
                buffer[y + 1][x + symbolSize] = d1C;
                buffer[y + 2][x + symbolSize + 1] = 'H';
            }
            else if (buffer[y + 2][x - 2] == ' ')
            {
                buffer[y + 1][x - 1] = d2C;
                buffer[y + 2][x - 2] = 'H';
            }
            else if (buffer[y - 2][x + symbolSize + 1] == ' ')
            {
                buffer[y - 1][x + symbolSize] = d2C;
                buffer[y - 2][x + symbolSize + 1] = 'H';
            }
            else if (buffer[y - 2][x - 2] == ' ')
            {
                buffer[y - 1][x - 1] = d1C;
                buffer[y - 2][x - 2] = 'H';
            }
            else
            {
                Log<MolecularStructure>().warn("Incomplete ASCII print.");
                break;
            }
        }
    }
}

std::string MolecularStructure::print() const
{
    if (atoms.empty())
    {
        if (isVirtualHydrogen())
            return "HÄH\n";
        return "";
    }

    TextBlock buffer(200, 50);
    std::vector<uint8_t> visited(atoms.size(), false);
    rPrint(buffer, buffer.getWidth() / 4, buffer.getHeight() / 2, *atoms.front(), visited, !isOrganic());

    buffer.trim();
    return buffer.toString();
}

std::string MolecularStructure::printInfo() const
{
    std::stringstream info;
    const auto atomCount = atoms.size();
    
    info << "SMILES: " << toSMILES() << "\n";
    StringTable adjacencyTable({ "Id", "Atom", "Bonds" }, false);

    size_t maxSymbolSize = 0;
    for (size_t i = 0; i < atomCount; ++i)
    {
        const auto& atom = *atoms[i];
        std::stringstream bondStr;
        if (atom.bonds.size() > 1)
        {
            for (auto b = atom.bonds.begin(); b != atom.bonds.end() - 1; ++b)
                bondStr << b->getSMILES() << std::to_string(b->getOther().index) << ", ";
        }
        bondStr << atom.bonds.back().getSMILES() << std::to_string(atom.bonds.back().getOther().index);

        maxSymbolSize = std::max(maxSymbolSize, atom.getAtom().getSymbolStr().size());
        adjacencyTable.addEntry({ std::to_string(i), atom.getAtom().getSymbolStr(), bondStr.str() });
    }

    info << "Stats:\n";
    const auto radicalAtomCount = getRadicalAtomsCount();
    info << " - Atom count:         " << getTotalAtomCount()
        << " (concrete: " << atomCount - radicalAtomCount
        << ", radical: " << radicalAtomCount
        << ", implied: " << impliedHydrogenCount << ")\n";

    const auto atomHistogram = getComponentCountMap();
    for (const auto& [symbol, count] : atomHistogram)
        info << "      " << std::format("{:>{}}", symbol.getString(), maxSymbolSize) << " : " << count << '\n';

    const auto bondCount = getBondCount();
    info << " - Bond count:         " << bondCount + impliedHydrogenCount
        << " (concrete: " << bondCount
        << ", implied: " << impliedHydrogenCount << ")\n";
    info << " - Bond sparsity:      " << std::format("{:.1f}", (1.0 - bondCount / ((atomCount * (atomCount - 1)) / 2.0)) * 100.0) << "%\n";

    info << " - Molar mass:         " << getMolarMass().toString() << '\n';
    info << " - Cycle count:        " << getCycleCount() << '\n';
    info << " - Is organic:         " << (isOrganic() ? "Yes" : "No") << '\n';
    info << " - Degrees of freedom: " << std::to_string(getDegreesOfFreedom()) << '\n';

    info << "Adjacency table:\n";
    adjacencyTable.dump(info);
    
    return info.str();
}
