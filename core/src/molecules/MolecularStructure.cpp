#include "molecules/MolecularStructure.hpp"

#include "data/def/Parsers.hpp"
#include "utils/ASCII.hpp"
#include "io/StringTable.hpp"
#include "utils/Bin.hpp"
#include "utils/Path.hpp"
#include "io/Log.hpp"

#include <boost/dynamic_bitset.hpp>

#include <array>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <numeric>
#include <fstream>

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

MolecularStructure MolecularStructure::createCopy() const
{
    return MolecularStructure(*this);
}

void MolecularStructure::clear()
{
    atoms.clear();
    impliedHydrogenCount = 0;
}

void MolecularStructure::addBond(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType)
{
    from.bonds.emplace_back(to, bondType);
    to.bonds.emplace_back(from, bondType);
}

bool MolecularStructure::addBondChecked(BondedAtomBase& from, BondedAtomBase& to, const BondType bondType)
{
    if (from.isSame(to))
        return false;
    if (from.getBondTo(to) != nullptr)
        return false; // Bond already exists.

    addBond(from, to, bondType);
    return true;
}

BondedAtomBase* MolecularStructure::addAtom(const Symbol& symbol, BondedAtomBase* prev, const BondType bondType)
{
    // Ignore implied atoms.
    static const auto hydrogen = Atom("H");
    if (symbol == hydrogen.getData().symbol && bondType == BondType::SINGLE)
        return prev;

    auto& inserted = *atoms.emplace_back(BondedAtomBase::create(symbol, static_cast<c_size>(atoms.size()), {}));
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

namespace
{
    // Returns the number of bonds connected to an atom (non-single bonds are taken into account)
    uint8_t getDegreeOf(const BondedAtomBase& atom)
    {
        uint8_t cnt = 0;
        for (const auto& b : atom.bonds)
            cnt += b.getValence();
        return cnt;
    }
}

int8_t MolecularStructure::getImpliedHydrogenCount(const BondedAtomBase& atom)
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
        const auto h = getImpliedHydrogenCount(*a);
        if (h == -1)
            return -1;

        hCount += h;
    }
    return hCount;
}

bool MolecularStructure::isFullyConnected() const
{
    if (atoms.size() == 0)
        return true;
    if (getBondCount() < atoms.size() - 1) // Minimum number of edges: N - 1
        return false;

    c_size visitedCount = 0;
    std::vector<uint8_t> visited(atoms.size(), false);

    std::stack<c_size> stack;
    stack.push(atoms.front()->index);
    do
    {
        const auto index = stack.top();
        stack.pop();
        if (visited[index])
            continue;

        ++visitedCount;
        visited[index] = true;
        
        const auto& atom = *atoms[index];
        for (const auto& b : atom.bonds)
        {
            const auto& other = b.getOther();
            if (visited[other.index])
                continue;

            if (other.getBondTo(atom) == nullptr)
                return false; // Unidirectional bond.

            stack.push(other.index);
        }
    } while (stack.size());

    return visitedCount == atoms.size();
}

//
// SMILES
//

std::optional<MolecularStructure> MolecularStructure::fromSMILES(const std::string& smiles)
{
    MolecularStructure temp;
    return temp.loadFromSMILES(smiles) ? std::optional(std::move(temp)) : std::nullopt;
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
            if (i < smiles.size() - 1)
            {
                if (Symbol symbol(smiles.substr(i, 2)); Atom::isDefined(symbol))
                {
                    prev = addAtom(symbol, prev, bondType);
                    bondType = BondType::SINGLE;
                    ++i;
                    continue;
                }
            }

            if (Symbol symbol(smiles[i]); Atom::isDefined(symbol))
            {
                prev = addAtom(symbol, prev, bondType);
                bondType = BondType::SINGLE;
                continue;
            }

            Log(this).error("Undefined atomic symbol '{}' in SMILES:\n{}\n{}^", smiles[i], smiles, std::string(i, ' '));
            clear();
            return false;
        }

        // Branch begin
        if (smiles[i] == '(')
        {
            if (not prev)
            {
                Log(this).error("Branch begin token: '(' precedes all atoms in SMILES:\n{}\n{}^", smiles, std::string(i, ' '));
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
                Log(this).error("Unpaired token: ')' in SMILES:\n{}\n{}^", smiles, std::string(i, ' '));
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
            const auto t = smiles.find(']', i + 1);
            if (t == std::string::npos)
            {
                Log(this).error("Unpaired '[' token in SMILES:\n{}\n{}^", smiles, std::string(i, ' '));
                clear();
                return false;
            }

            const Symbol symbol(smiles.substr(i + 1, t - i - 1));
            if (not Atom::isDefined(symbol))
            {
                Log(this).error("Undefined atomic symbol '{}' in SMILES:\n{}\n{}^", symbol, smiles, std::string(i, ' '));
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
                Log(this).error("Ring label precedes all atoms in SMILES:\n{}\n{}^", smiles, std::string(i, ' '));
                clear();
                return false;
            }

            if (i + 2 >= smiles.size() || not isdigit(smiles[i + 1]) || not isdigit(smiles[i + 2]))
            {
                Log(this).error("Missing or invalid multi-digit ring label in SMILES:\n{}\n{}^", smiles, std::string(i, ' '));
                clear();
                return false;
            }

            // int8_t is enough for two digits
            const uint8_t label = (smiles[i + 1] - '0') * 10 + smiles[i + 2] - '0';
            i += 2;

            const auto ringIt = rings.find(label);
            if (ringIt == rings.end())
            {
                // New label.
                rings.emplace(label, static_cast<c_size>(atoms.size() - 1));
                continue;
            }

            if (prev == nullptr)
            {
                Log(this).error("Illegal ring-closing bond with label: '{}' in SMILES:\n{}\n{}^", label, smiles, std::string(i, ' '));
                clear();
                return false;
            }

            if (not addBondChecked(*prev, *atoms[rings[label]], bondType))
            {
                Log(this).error("Cycle closure with label '{}' redefines an existing bond between two atoms in SMILES:\n{}\n{}^", label, smiles, std::string(i, ' '));
                clear();
                return false;
            }

            // Some SMILES generators prefer to reuse labels instead of using double digit labels.
            rings.erase(ringIt);

            bondType = BondType::SINGLE;
            continue;
        }

        // Single digit ring label
        if (isdigit(smiles[i]))
        {
            if (not prev)
            {
                Log(this).error("Ring label precedes all atoms in SMILES:\n{}\n{}^", smiles, std::string(i, ' '));
                clear();
                return false;
            }

            const uint8_t label = smiles[i] - '0';

            const auto ringIt = rings.find(label);
            if (ringIt == rings.end())
            {
                // New label.
                rings.emplace(label, static_cast<c_size>(atoms.size() - 1));
                continue;
            }
            
            if (not addBondChecked(*prev, *atoms[rings[label]], bondType))
            {
                Log(this).error("Cycle closure with label '{}' redefines an existing bond between two atoms in SMILES:\n{}\n{}^", label, smiles, std::string(i, ' '));
                clear();
                return false;
            }

            // Some SMILES generators prefer to reuse labels instead of using double digit labels.
            rings.erase(ringIt);

            bondType = BondType::SINGLE;
            continue;
        }

        Log(this).error("Unrecognized symbol '{}' in SMILES:\n{}\n{}^", smiles[i], smiles, std::string(i, ' '));
        clear();
        return false;
    }

    if(branches.empty() == false)
    {
        Log(this).error("Unpaired token: ')' in SMILES:\n{}", smiles);
        clear();
        return false;
    }

    const auto hCount = countImpliedHydrogens();
    if(hCount == -1)
    {
        Log(this).error("Valence of an atom was exceeded in SMILES:\n{}", smiles);
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;

    canonicalize();

    return true;
}

namespace
{

std::optional<std::pair<ASCII::Position, std::string>> extractSymbol(const TextBlock& buffer, const ASCII::Position origin)
{
    // The symbol may reside on both sides of the origin:
    //   |
    //   |#
    //  #|
    //  #|#
    //   |##
    // ##|
    // ##|#
    // ...
    const auto& line = buffer[origin.y];

    int16_t beginIdx = origin.x - 1;
    while (not line.isWhiteSpace(beginIdx) && line[beginIdx] != 'H') --beginIdx;
    int16_t endIdx = origin.x + 1;
    while (not line.isWhiteSpace(endIdx) && line[endIdx] != 'H') ++endIdx;

    for (int16_t b = beginIdx + 1; b <= origin.x; ++b)
    {
        const auto startPoint = ASCII::Position(b, origin.y);
        for (int16_t e = endIdx - 1; e >= origin.x; --e)
        {
            auto section = buffer[ASCII::PositionLine(startPoint, e - b + 1)];
            if (buffer[startPoint] == '%' && def::parse<c_size>(section.substr(1)))
                return std::make_pair(startPoint, std::move(section));
            if (Atom::isDefined(section))
                return std::make_pair(startPoint, std::move(section));
        }
    }

    return std::nullopt;
}

ASCII::Position findStartPosition(const TextBlock& buffer)
{
    // Finds the first atom symbol in the buffer.
    for (auto y = buffer.beginIdx(); y < buffer.endIdx(); ++y)
    {
        const auto& line = buffer[y];
        for (auto x = line.beginIdx(); x < line.endIdx(); ++x)
        {
            const auto origin = ASCII::Position(x, y);

            auto endIdx = checked_cast<int16_t>(x);
            while (not line.isWhiteSpace(endIdx))
            {
                const auto section = buffer[ASCII::PositionLine(origin, checked_cast<Symbol::SizeT>(endIdx - x + 1))];
                if (Atom::isDefined(section))
                    return static_cast<ASCII::Position>(origin);

                ++endIdx;
            }
        }
    }

    return utils::npos<ASCII::Position>;
}

void insertError(TextBlock& buffer, const ASCII::Position position)
{
    static constexpr std::array<std::pair<ASCII::Direction, char>, 4> directions
    {
        std::make_pair(ASCII::Direction::Down, '^'),
        std::make_pair(ASCII::Direction::Up, 'v'),
        std::make_pair(ASCII::Direction::Right, '<'),
        std::make_pair(ASCII::Direction::Left, '>')
    };

    for (const auto [dir, chr] : directions)
    {
        const auto pos = position + dir.get();
        if (buffer.isWhiteSpace(pos))
        {
            buffer[pos] = chr;
            return;
        }
    }
}

} // namespace

std::optional<MolecularStructure> MolecularStructure::fromASCII(const std::string& ascii)
{
    MolecularStructure temp;
    return temp.loadFromASCII(ascii) ? std::optional(std::move(temp)) : std::nullopt;
}

bool MolecularStructure::loadFromASCII(const std::string& ascii)
{
    TextBlock buffer(ascii);

    if(buffer.empty())
    {
        Log(this).error("Received empty ASCII block.");
        return false;
    }

    if (buffer.toString() == "H2")
    {
        impliedHydrogenCount = 2;
        return true;
    }

    const auto startPos = findStartPosition(buffer);
    if (utils::isNPos(startPos))
    {
        Log(this).error("ASCII block contains no recognizable atom symbols:\n{0}.", buffer.toString());
        return false;
    }

    std::unordered_map<ASCII::Position, BondedAtomBase*> positionMap;
    std::unordered_map<c_size, BondedAtomBase*> ringClosures;

    class State
    {
    public:
        ASCII::Position position;
        BondType bondType;
        BondedAtomBase* prev;

        State(
            const ASCII::Position position,
            const BondType bondType,
            BondedAtomBase* prev
        ) noexcept :
            position(position),
            bondType(bondType),
            prev(prev)
        {}
    };

    std::stack<State> stack;
    stack.emplace(checked_cast<int16_t>(startPos), BondType::NONE, nullptr);
    while (stack.size())
    {
        auto state = stack.top();
        stack.pop();

        // In some rare cases a cycle closure might only be apparent from one direction:
        //   Si-C     .
        //  /    \    .
        // C      C   .
        //  \     /   .
        //   Si-Si    .
        // When closing a cycle this way, we must check the closure bond doesn't already exist.
        if (const auto atomIt = positionMap.find(state.position); atomIt != positionMap.end())
        {
            // Close cycle.
            addBondChecked(*state.prev, *atomIt->second, state.bondType);
            continue;
        }

        auto extractedSymbol = extractSymbol(buffer, state.position);
        if (not extractedSymbol)
        {
            insertError(buffer, state.position);
            Log(this).error("Failed to parse ASCII atom symbol originating at {0}:\n{1}", state.position, buffer.toString());
            clear();
            return false;
        }

        state.position = extractedSymbol->first;

        if (extractedSymbol->second.starts_with('%'))
        {
            const auto label = *def::parse<c_size>(extractedSymbol->second.substr(1));
            auto it = ringClosures.find(label);
            if (it == ringClosures.end())
            {
                ringClosures.emplace(label, state.prev);
                continue;
            }

            if (it->second == nullptr)
            {
                insertError(buffer, state.position);
                Log(this).error("Found duplicate cycle closure label '{0}' at {1}:\n{2}", label, state.position, buffer.toString());
                clear();
                return false;
            }

            if (not addBondChecked(*state.prev, *it->second, state.bondType))
            {
                insertError(buffer, state.position);
                Log(this).error("Cycle closure with label '{0}' at {1} redefines an existing bond between two atoms:\n{2}", label, state.position, buffer.toString());
                clear();
                return false;
            }

            it->second = nullptr;
            continue;
        }

        const Symbol symbol(std::move(extractedSymbol->second));

        auto* newAtom = addAtom(symbol, state.prev, state.bondType);
        for(Symbol::SizeT i = 0; i < symbol.size(); ++i)
            positionMap.emplace(state.position + ASCII::Direction::Right.get() * i, newAtom);

        const auto nextDirections = ASCII::StructurePrinter::getPossibleNextDirections(ASCII::PositionLine(state.position, symbol.size()));
        for (const auto [dir, origin] : nextDirections)
        {
            const auto edgePos = ASCII::Position(origin + dir.get());
            auto nodePos = ASCII::Position(edgePos + dir.get());
            
            const auto nextBondType = Bond::fromASCII(buffer[edgePos]);
            if (nextBondType == BondType::NONE)
                continue; // Not a bond.

            if (not Bond::isInDirection(buffer[edgePos], dir))
                continue;

            if (const auto atomIt = positionMap.find(nodePos); atomIt != positionMap.end())
            {
                if (atomIt->second == state.prev)
                    continue; // Direction goes backward.

                // Close cycle.
                addBond(*newAtom, *atomIt->second, nextBondType);
                continue;
            }

            if (buffer.isWhiteSpace(nodePos))
            {
                // Multi-character symbols may shift the position by 1:
                //  \    .
                //   C   .
                //   /   .
                // Si    .
                const auto retryPos = static_cast<ASCII::Position>(nodePos + ASCII::Direction::Left.get());

                if (buffer.isWhiteSpace(retryPos))
                {
                    //insertError(buffer, nodePos);
                    //insertError(buffer, retryPos);

                    //Log(this).error("Expected atom symbol at {0} or {1}:\n{2}", nodePos, retryPos, buffer.toString());
                    //clear();
                    //return false;
                    continue;
                }

                nodePos = retryPos;
            }

            stack.emplace(nodePos, nextBondType, newAtom);
        }
    }

    const auto hCount = countImpliedHydrogens();
    if (hCount == -1)
    {
        Log(this).error("Valence of an atom was exceeded is ASCII block:\n{0}", ascii);
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

const BondedAtomBase& MolecularStructure::getBondedAtom(const c_size idx) const
{
    return *atoms[idx];
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
    auto i = static_cast<c_size>(atoms.size());
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

            if (getImpliedHydrogenCount(*a) > 0)
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
    return static_cast<c_size>(atoms.size());
}

c_size MolecularStructure::getTotalAtomCount() const
{
    return static_cast<c_size>(atoms.size() + impliedHydrogenCount);
}

c_size MolecularStructure::getBondCount() const
{
    c_size cnt = 0;
    for (const auto& a : atoms)
        cnt += static_cast<c_size>(a->bonds.size());

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

namespace
{
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
        const std::vector<uint8_t>& visitedB,
        const std::unordered_map<c_size, c_size>& mapping,
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
        std::array<int8_t, BondType::BOND_TYPE_COUNT + 1> counts{ 0 };
        for (c_size i = 0; i < otherA.bonds.size(); ++i)
        {
            ++counts[otherA.bonds[i].getType()];
            --counts[otherB.bonds[i].getType()];
            counts.back() += mapping.contains(otherA.bonds[i].getOther().index);
            counts.back() -= visitedB[otherB.bonds[i].getOther().index];
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
                    not areMatching(bondA, bondB, visitedB, mapping, escapeRadicalTypes))
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

namespace
{
    uint8_t getBondSimilarity(const BondedAtomBase& a, const BondedAtomBase& b)
    {
        uint8_t score = 255;
        std::array<int8_t, BondType::BOND_TYPE_COUNT> counts{ 0 };

        for (const auto& bondA : a.bonds)
            ++counts[bondA.getType()];
        for (const auto& bondB : b.bonds)
            --counts[bondB.getType()];

        const auto scorePerBond = static_cast<uint8_t>(a.bonds.size()) / 255;
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
        sdMapping.emplace(c, static_cast<c_size>(destination.atoms.size()));
        destination.atoms.emplace_back(BondedAtomBase::create(source.atoms[c]->getAtom(), static_cast<c_size>(destination.atoms.size()), {}));

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

namespace
{
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
        return closures.emplace(idxA, idxB, static_cast<c_size>(closures.size() + 1)).second;
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
}

//
// Serialize to SMILES
//

namespace
{
    std::string getCycleTagString(const c_size tag)
    {
        return tag < 10 ?
            std::to_string(tag) :
            '%' + std::to_string(tag);
    }

    void rToSMILES(
        const BondedAtomBase* current, const BondedAtomBase* prev,
        std::vector<c_size>& insertPositions, CycleClosureSet& cycleClosures,
        std::string& smiles
    );

    void inline rNextToSMILES(
        const BondedAtomBase* current, const Bond& bondToNext,
        CycleClosureSet& cycleClosures, std::vector<c_size>& insertPositions,
        std::string& smiles)
    {
        const auto next = &bondToNext.getOther();
        if (not utils::isNPos(insertPositions[next->index]))
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
        CycleClosureSet& cycleClosures, std::vector<c_size>& insertPositions,
        std::string& smiles)
    {
        const auto next = &bondToNext.getOther();
        if (not utils::isNPos(insertPositions[next->index]))
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
        std::vector<c_size>& insertPositions, CycleClosureSet& cycleClosures,
        std::string& smiles)
    {
        while (true)
        {
            smiles += current->getAtom().getSMILES();
            insertPositions[current->index] = static_cast<c_size>(smiles.size());

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
}

std::string MolecularStructure::toSMILES(const c_size startAtomIdx) const
{
    if (isVirtualHydrogen())
        return "HH";

    if (atoms.empty())
        return "";

    std::string smiles;
    smiles.reserve(atoms.size());

    const auto current = atoms[startAtomIdx].get();
    smiles += current->getAtom().getSMILES();

    const auto neighbourCount = current->bonds.size();
    if (neighbourCount == 0)
        return smiles;

    std::vector<c_size> insertPositions(atoms.size(), utils::npos<c_size>);
    insertPositions[startAtomIdx] = static_cast<c_size>(smiles.size());
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

        if (utils::isNPos(insertPositions[last->index]))
        {
            // Since current is the first atom we don't have to add a new cycle closure, we already know this isn't a new cycle.
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
// MolBin Serialization
//

std::optional<MolecularStructure> MolecularStructure::loadMolBinFile(const std::string& path)
{
    const auto normPath = utils::normalizePath(path);
    std::ifstream is(normPath);
    if (not is)
    {
        Log<MolecularStructure>().error("Failed to open file: '{}' for reading.", normPath);
        return std::nullopt;
    }

    return fromMolBin(is);
}

std::optional<MolecularStructure> MolecularStructure::fromMolBin(std::istream& is)
{
    MolecularStructure temp;
    return temp.loadFromMolBin(is) ? std::optional(std::move(temp)) : std::nullopt;
}

bool MolecularStructure::loadFromMolBin(std::istream& is)
{
    if (not is)
    {
        Log(this).error("MolBin stream already reached EOF or is invalid.");
        return false;
    }

    if (is.peek() == '!')
    {
        impliedHydrogenCount = 2;
        return true;
    }

    // Must save all bonds until all atoms are added in order to preserve the same bond order.
    std::vector<std::vector<std::pair<BondType, c_size>>> futureBonds;

    do
    {
        char chr;

        std::string symbolStr;
        while (is.get(chr) && chr != ':' && chr != ';')
            symbolStr += chr;
        if (symbolStr.empty())
            break;

        Symbol symbol(std::move(symbolStr));
        if (not Atom::isDefined(symbol))
        {
            Log(this).error("MolBin atomic symbol: '{}' is undefined.", symbol);
            clear();
            return false;
        }

        atoms.emplace_back(BondedAtomBase::create(symbol, static_cast<c_size>(atoms.size()), {}));
        futureBonds.emplace_back();

        if (not is)
            break;
        if (chr == ';')
            continue;

        do
        {
            const auto bond = bin::parse<std::pair<BondType, c_size>>(is);
            if (not bond)
            {
                Log(this).error("Failed to parse MolBin bond for symbol: '{}'.", symbol);
                clear();
                return false;
            }

            const auto [bondType, otherIdx] = *bond;
            if(bondType <= BondType::NONE || bondType >= BondType::BOND_TYPE_COUNT)
            {
                Log(this).error("Invalid MolBin bond type: {} on symbol: '{}'.", underlying_cast(bondType), symbol);
                clear();
                return false;
            }
            if (otherIdx == atoms.size() - 1)
            {
                Log(this).error("Self-pointing MolBin (self: {}) on symbol: '{}'.", otherIdx, symbol);
                clear();
                return false;
            }

            futureBonds.back().emplace_back(bondType, otherIdx);

            if (is.peek() == ';')
            {
                is.ignore(1);
                break;
            }

        } while (is && is.peek() != std::char_traits<char>::eof());
    } while (is);

    for (c_size i = 0; i < futureBonds.size(); ++i)
    {
        for (const auto [type, other] : futureBonds[i])
        {
            if (other >= atoms.size())
            {
                Log(this).error("MolBin bond other index: {} refers to a non-existing atom (max index: {}).", other, atoms.size());
                clear();
                return false;
            }
            if (atoms[i]->getBondTo(*atoms[other]) != nullptr)
            {
                Log(this).error("MolBin bond redefines an existing bond between atoms {} and {}.", i, other);
                clear();
                return false;
            }

            atoms[i]->bonds.emplace_back(*atoms[other], type);
        }
    }

    if(not isFullyConnected())
    {
        Log(this).error("MolBin representation isn't fully connected.");
        clear();
        return false;
    }

    const auto hCount = countImpliedHydrogens();
    if (hCount == -1)
    {
        Log(this).error("Valence of an atom was exceeded in MolBin representation.");
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;

    // No canonicalization is needed for MolBin representation.
    return true;
}

void MolecularStructure::toMolBin(std::ostream& os) const
{
    // MolBin:
    // !                                      | Virtual hydrogen: H2
    // [symbol]:[bond_type][other_index]...\n | Others
    if (isVirtualHydrogen())
    {
        os << '!';
        return;
    }

    const auto dumpAtom = [&os](const BondedAtomBase& atom)
        {
            os << atom.getAtom().getSymbol();

            const auto bondCount = static_cast<c_size>(atom.bonds.size());
            if (bondCount != 0)
            {
                os << ':';
                for (const auto& b : atom.bonds)
                    bin::print(os, std::make_pair(b.getType(), b.getOther().index));
            }
        };

    for (c_size a = 0; a < atoms.size() - 1; ++a)
    {
        dumpAtom(*atoms[a]);
        os << ';';
    }
    dumpAtom(*atoms.back());
}

void MolecularStructure::toMolBinFile(const std::string& path) const
{
    const auto normPath = utils::normalizePath(path);
    std::ofstream os(normPath);
    if (not os)
    {
        Log<MolecularStructure>().error("Failed to open file: '{}' for writing.", normPath);
        return;
    }

    toMolBin(os);
    os.close();
}

//
// ASCII Print
//

namespace
{
    using Edge = UndirectedEdge<c_size>;

    class CycleDecoder
    {
    private:
        const ::MolecularStructure& owningStructure;
        MolecularStructure::Cycle decodedCycle;
        std::vector<Edge> unboundEdges;

        bool add(const c_size idx);

    public:
        CycleDecoder(const ::MolecularStructure& owningStructure) noexcept;
        CycleDecoder(const CycleDecoder&) = delete;
        CycleDecoder(CycleDecoder&&) = default;

        bool add(const Edge edge);

        MolecularStructure::Cycle& getDecodedCycle();
    };

    CycleDecoder::CycleDecoder(const ::MolecularStructure& owningStructure) noexcept :
        owningStructure(owningStructure)
    {}

    bool CycleDecoder::add(const c_size idx)
    {
        decodedCycle.emplace_back(&owningStructure.getBondedAtom(idx));

        size_t i = 0;
        while (i < unboundEdges.size())
        {
            const auto edge = unboundEdges[i];
            if (edge.getIdxA() == decodedCycle.back()->index)
            {
                if (edge.getIdxB() == decodedCycle.front()->index)
                    return true; // Cycle end

                decodedCycle.emplace_back(&owningStructure.getBondedAtom(edge.getIdxB()));
                utils::swapAndPop(unboundEdges, i);
                i = 0;
                continue;
            }
            if (edge.getIdxB() == decodedCycle.back()->index)
            {
                if (edge.getIdxA() == decodedCycle.front()->index)
                    return true; // Cycle end

                decodedCycle.emplace_back(&owningStructure.getBondedAtom(edge.getIdxA()));
                utils::swapAndPop(unboundEdges, i);
                i = 0;
                continue;
            }

            ++i;
        }

        return false;
    }

    bool CycleDecoder::add(const Edge edge)
    {
        if (decodedCycle.empty())
        {
            // First edge
            decodedCycle.emplace_back(&owningStructure.getBondedAtom(edge.getIdxA()));
            decodedCycle.emplace_back(&owningStructure.getBondedAtom(edge.getIdxB()));
            return false;
        }

        // Add continuous edge at the end
        if (edge.getIdxA() == decodedCycle.back()->index)
        {
            if (edge.getIdxB() == decodedCycle.front()->index)
                return true; // Cycle end

            return add(edge.getIdxB());
        }
        if (edge.getIdxB() == decodedCycle.back()->index)
        {
            if (edge.getIdxA() == decodedCycle.front()->index)
                return true; // Cycle end

            return add(edge.getIdxA());
        }

        unboundEdges.emplace_back(edge);
        return false;
    }

    MolecularStructure::Cycle& CycleDecoder::getDecodedCycle()
    {
        return decodedCycle;
    }
}

std::vector<MolecularStructure::Cycle> MolecularStructure::getFundamentalCycleBasis() const
{
    // The cycle count is easy to compute and can be used to stop the algorithm early.
    const auto cycleCount = getCycleCount();
    if (cycleCount == 0)
        return std::vector<Cycle>();

    // Paton's Algorithm.
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

    std::vector<Cycle> cycles;
    std::unordered_set<Edge> cycleClosures;
    cycles.reserve(cycleCount);
    cycleClosures.reserve(cycleCount);

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
            if (cycleClosures.contains(Edge(c, nextIdx)))
                continue;

            // New cycle
            Cycle cycle;

            // Since we use DFS to traverse we can just intersect root->...->parents[C]->C with parents[N]->N.
            // The intersection combined with C->N yields the cycle.
            for (c_size i = parents[c]; i != parents[nextIdx]; i = parents[i])
                cycle.emplace_back(atoms[i].get());

            cycle.emplace_back(atoms[parents[nextIdx]].get());
            cycle.emplace_back(atoms[nextIdx].get());
            cycle.emplace_back(atoms[c].get());

            cycles.emplace_back(std::move(cycle));
            if (cycles.size() == cycleCount)
                return cycles;

            cycleClosures.emplace(c, nextIdx);
        }
    }

    return cycles;
}

std::vector<MolecularStructure::Cycle> MolecularStructure::getMinimalCycleBasis() const
{
    auto fundamentalCycles = getFundamentalCycleBasis();
    if (fundamentalCycles.empty())
        return std::vector<Cycle>();

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

    for (const auto& cycle : fundamentalCycles)
    {
        const Edge edge(cycle.front()->index, cycle.back()->index);
        if (not edgeLabels.contains(edge))
        {
            const auto newLabel = static_cast<c_size>(edgeLabels.size());
            edgeLabels.emplace(edge, newLabel);
            uniqueEdges.emplace_back(edge);
        }

        for (size_t i = 0; i < cycle.size() - 1; ++i)
        {
            const Edge edge(cycle[i]->index, cycle[i + 1]->index);
            if (edgeLabels.contains(edge))
                continue;

            const auto newLabel = static_cast<c_size>(edgeLabels.size());
            edgeLabels.emplace(edge, newLabel);
            uniqueEdges.emplace_back(edge);
        }
    }

    std::vector<Cycle> result;
    result.reserve(fundamentalCycles.size());

    // Encode each cycle C as a bitset B where B[i] = 1 if the cycle contains the i-th unique bond,
    // then XOR it with all the previous cycles (which are smaller because of sorting) resulting in
    // an fully independent cycle.
    std::vector<boost::dynamic_bitset<uint32_t>> basis;
    basis.reserve(fundamentalCycles.size());

    for (const auto& cycle : fundamentalCycles)
    {
        boost::dynamic_bitset<uint32_t> encodedCycle(uniqueEdges.size());
        const auto edgeIdx = edgeLabels.find(Edge(cycle.front()->index, cycle.back()->index))->second;
        encodedCycle.set(edgeIdx);
        for (size_t i = 0; i < cycle.size() - 1; ++i)
        {
            const auto edgeIdx = edgeLabels.find(Edge(cycle[i]->index, cycle[i + 1]->index))->second;
            encodedCycle.set(edgeIdx);
        }

        for (size_t i = 0; i < basis.size(); ++i)
        {
            auto xorCycle = encodedCycle ^ basis[i];

            // Even if the XOR'd cycle has the same size as the initial cycle, the XOR'd one,
            // unless equal, should be more independent towards previous cycles in the basis.
            if (xorCycle.count() > encodedCycle.count() || xorCycle == encodedCycle)
                continue;
            
            encodedCycle = std::move(xorCycle);

            // Loopback to ensure complete reduction. This is needed in very niche cases like:
            // "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", there might be better solutions.
            for (size_t j = 0; j < i; ++j)
            {
                xorCycle = encodedCycle ^ basis[j];
                if (xorCycle.count() > encodedCycle.count())
                    continue;

                encodedCycle = std::move(xorCycle);
            }
        }

        if (encodedCycle.none()) // The fundamental and minimal cycle bases should always have the same number of cycles.
            Log(this).fatal("Received a fully dependent cycle as a fundamental cycle.");

        basis.emplace_back(std::move(encodedCycle));
    }

    // Decode the cycles.
    for (const auto& cycle : basis)
    {
        CycleDecoder decoder(*this);
        for (size_t i = 0; i < cycle.size(); ++i)
        {
            if (not cycle.test(i))
                continue;
            if (decoder.add(uniqueEdges[i]))
                break; // Cycle was closed.
        }

        result.emplace_back(std::move(decoder.getDecodedCycle()));
    }

    return result;
}

ColoredTextBlock MolecularStructure::toASCII(const ASCII::PrintOptions options) const
{
    if (isVirtualHydrogen())
        return ColoredTextBlock(ColoredString("H2", OS::Color::Grey));

    if (atoms.empty())
        return ColoredTextBlock("");

    static constexpr OS::ColorType rareAtomColor = OS::Color::White;
    static constexpr OS::ColorType radicalAtomColor = OS::Color::Cyan;
    static const std::unordered_map<Symbol, OS::ColorType> defaultColorMap
    {
        { Symbol("C"), OS::Color::Grey },
        { Symbol("Si"), OS::Color::Grey },
        { Symbol("O"), OS::Color::Red },
        { Symbol("N"), OS::Color::Blue },
        { Symbol("S"), OS::Color::Yellow },
        { Symbol("P"), OS::Color::Magenta },
        { Symbol("F"), OS::Color::DarkYellow },
        { Symbol("Cl"), OS::Color::Green },
        { Symbol("Br"), OS::Color::DarkRed },
        { Symbol("I"), OS::Color::DarkMagenta },
    };

    auto cycles = utils::transform<ASCII::Cycle>(getMinimalCycleBasis());

    // Populate nodes.
    std::vector<ASCII::Node> nodes;
    nodes.reserve(atoms.size());
    for (const auto& a : atoms)
    {
        const auto maybeColor = utils::find(defaultColorMap, a->getAtom().getSymbol());
        const auto nodeColor =
            maybeColor ? *maybeColor :
            a->getAtom().isRadical() ? radicalAtomColor :
            rareAtomColor;
        
        // Index symbols are padded to match the size of the corresponding atom symbol:
        // Symbol: Uup
        // Index:  1__
        const auto& atomSymbol = a->getAtom().getSymbol().str();

        auto symbol = options.has(ASCII::PrintFlags::PRINT_ATOM_INDICES) ?
            ColoredString(utils::padRight(utils::toBase<36>(a->index), atomSymbol.size(), '_'), nodeColor) :
            ColoredString(atomSymbol, nodeColor);

        nodes.emplace_back(*a, std::move(symbol));
    }

    // Populate edges.
    std::unordered_map<Edge, ASCII::Edge> edges;
    edges.reserve(atoms.size() + cycles.size() - 1); // N - 1 + C
    for (const auto& a : atoms)
    {
        const auto thisColor = nodes[a->index].getSymbolColor();

        for (const auto& b : a->bonds)
        {
            const auto otherIdx = b.getOther().index;

            if (not options.has(ASCII::PrintFlags::HIGHLIGHT_ATOM_ORIGIN))
            {
                edges.emplace(Edge(a->index, b.getOther().index), ASCII::Edge(b, OS::Color::DarkGrey));
                continue;
            }

            // The edge shares the color of its nodes.
            const auto otherColor = nodes[otherIdx].getSymbolColor();
            const auto color = thisColor == otherColor ? OS::darken(thisColor) : OS::Color::DarkGrey;

            edges.emplace(Edge(a->index, b.getOther().index), ASCII::Edge(b, color));
        }
    }

    ASCII::StructurePrinter printer(std::move(nodes), std::move(cycles), std::move(edges), options);
    printer.print();
    return std::move(printer.getBlock());
}

std::string MolecularStructure::printInfo() const
{
    std::ostringstream info;
    const auto atomCount = atoms.size();
    
    info << "SMILES: " << toSMILES() << '\n';
    info << "Structure:\n" << toASCII(ASCII::PrintOptions::Default | ASCII::PrintFlags::PRINT_IMPLIED_NON_CARBON_HYDROGENS) << '\n';
    StringTable adjacencyTable({ "Id", "Atom", "Bonds" }, false);

    uint8_t maxSymbolSize = 0;
    for (size_t i = 0; i < atomCount; ++i)
    {
        const auto& atom = *atoms[i];
        std::ostringstream bondStr;
        if (atom.bonds.size() > 1)
        {
            for (auto b = atom.bonds.begin(); b != atom.bonds.end() - 1; ++b)
                bondStr << b->getSMILES() << std::to_string(b->getOther().index) << ", ";
        }
        bondStr << atom.bonds.back().getSMILES() << std::to_string(atom.bonds.back().getOther().index);

        maxSymbolSize = std::max(maxSymbolSize, atom.getAtom().getSymbol().size());
        adjacencyTable.addEntry({ std::to_string(i), atom.getAtom().getSymbol().str(), bondStr.str()});
    }

    info << "Stats:\n";
    const auto radicalAtomCount = getRadicalAtomsCount();
    info << " - Atom count:         " << getTotalAtomCount()
        << " (concrete: " << atomCount - radicalAtomCount
        << ", radical: " << radicalAtomCount
        << ", implied: " << impliedHydrogenCount << ")\n";

    const auto atomHistogram = getComponentCountMap();
    for (const auto& [symbol, count] : atomHistogram)
        info << "      " << std::format("{:>{}}", symbol.str(), maxSymbolSize) << " : " << count << '\n';

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
