#include <array>
#include <algorithm>
#include <queue>

#include "MolecularStructure.hpp"
#include "Parsers.hpp"
#include "AtomFactory.hpp"
#include "TextBlock.hpp"
#include "Log.hpp"

MolecularStructure::MolecularStructure(const std::string& smiles)
{
    if (loadFromSMILES(smiles) == false)
    {
        Log(this).error("Molecule initialized with ill-formed smiles: \"{0}\".", smiles);
        return;
    }
    canonicalize();
}

MolecularStructure::MolecularStructure(const std::string& serialized, const bool canonicalize)
{
    deserialize(serialized);

    if(canonicalize)
        this->canonicalize();
}

MolecularStructure::MolecularStructure(const MolecularStructure& other) noexcept :
    impliedHydrogenCount(other.impliedHydrogenCount),
    bonds(other.bonds.size())
{
    this->atoms.reserve(other.atoms.size());
    for (size_t i = 0; i < other.atoms.size(); ++i)
        this->atoms.emplace_back(other.atoms[i]->clone());

    for (size_t i = 0; i < other.bonds.size(); ++i)
    {
        this->bonds[i].reserve(other.bonds[i].size());
        for (size_t j = 0; j < other.bonds[i].size(); ++j)
            this->bonds[i].emplace_back(other.bonds[i][j]);
    }
}

MolecularStructure::~MolecularStructure()
{
    clear();
}

bool MolecularStructure::loadFromSMILES(const std::string& smiles)
{
    clear();

    if (smiles == "HH") // the only purely virtual molecule
    {
        if (Atom::isDefined("H") == false)
            return false;

        impliedHydrogenCount = 2;
        return true;
    }

    std::unordered_map<uint8_t, c_size> rings;
    std::stack<c_size> branches;

    c_size prev = npos;
    BondType bondType = BondType::SINGLE;
    for(size_t i = 0; i < smiles.size(); ++i)
    {
        if (isalpha(smiles[i])) // basic organic atom identification
        {
            if (Symbol symbol(smiles[i]); Atom::isDefined(symbol))
            {
                atoms.emplace_back(AtomFactory::get(symbol));
            }
            else if (Symbol symbol(smiles.substr(i, 2)); Atom::isDefined(symbol))
            {
                atoms.emplace_back(AtomFactory::get(symbol));
                ++i;
            }
            else
            {
                Log(this).error("Atomic symbol '{0}' at {1} is undefined.", smiles[i], i);
                clear();
                return false;
            }

            bonds.emplace_back(std::vector<Bond>());

            if (prev != npos)
            {
                bonds[prev].emplace_back(Bond(atoms.size() - 1, bondType));
                bonds[bonds.size() - 1].emplace_back(Bond(prev, bondType));
                bondType = BondType::SINGLE;
            }

            prev = atoms.size() - 1;
            continue;
        }

        if (Bond::fromSMILES(smiles[i]) != BondType::NONE)
        {
            bondType = Bond::fromSMILES(smiles[i]);
            continue;
        }

        if (smiles[i] == '(')
        {
            branches.emplace(prev);
            continue;
        }

        if (smiles[i] == ')')
        {
            if (branches.empty())
            {
               Log(this).error("Unpaired ')' found at {0}.", i);
                clear();
                return false;
            }
            prev = branches.top();
            branches.pop();
            continue;
        }

        if (smiles[i] == '[') // special atom identification
        {
            const size_t t = smiles.find(']', i + 1);
            if (t == std::string::npos)
            {
                Log(this).error("Unpaired '[' found at {0}.", i);
                clear();
                return false;
            }

            const Symbol symbol(smiles.substr(i + 1, t - i - 1));
            if (Atom::isDefined(symbol) == false)
            {
                Log(this).error("Atomic symbol '{0}' at {1} is undefined.", symbol.getAsString(), i);
                clear();
                return false;
            }
            
            atoms.emplace_back(AtomFactory::get(symbol));
            bonds.emplace_back(std::vector<Bond>());

            if (prev != npos)
            {
                bonds[prev].emplace_back(Bond(atoms.size() - 1, bondType));
                bonds[bonds.size() - 1].emplace_back(Bond(prev, bondType));
                bondType = BondType::SINGLE;
            }

            prev = atoms.size() - 1;
            i = t;
            continue;
        }

        if (smiles[i] == '%')
        {
            if (i + 2 >= smiles.size() || isdigit(smiles[i + 1]) == false || isdigit(smiles[i + 2]) == false)
            {
                Log(this).error("Two-digit ring label is missing.");
                clear();
                return false;
            }

            uint8_t label = smiles[i + 1] * 10 + smiles[i + 2] - 11 * '0';
            if (rings.contains(label))
            {
                bonds[rings[label]].emplace_back(Bond(atoms.size() - 1, bondType));
                bonds[atoms.size() - 1].emplace_back(Bond(rings[label], bondType));
                bondType = BondType::SINGLE;
            }
            else
            {
                rings.emplace(std::move(std::make_pair(label, atoms.size() - 1)));
            }
            
            i += 2;
            continue;
        }

        if (isdigit(smiles[i]))
        {
            uint8_t label = smiles[i] - '0';
            if (rings.contains(label))
            {
                bonds[rings[label]].emplace_back(Bond(atoms.size() - 1, bondType));
                bonds[atoms.size() - 1].emplace_back(Bond(rings[label], bondType));
            }
            else
            {
                rings.emplace(std::move(std::make_pair(label, atoms.size() - 1)));
            }
            continue;
        }

        Log(this).error("Unidentified symbol '{0}' at {1} found.", smiles[i], i);
        clear();
        return false;
    }

    if(branches.empty() == false)
    {
        Log(this).error("Unpaired '(' found.");
        clear();
        return false;
    }

    removeUnnecessaryHydrogens();

    const auto hCount = countImpliedHydrogens();
    if(hCount == -1)
    {
        Log(this).error("Valence of an atom was exceeded.");
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;

    return true;
}

void MolecularStructure::canonicalize()
{
    if (atoms.size() <= 1)
        return;

    std::vector<const Atom*> copy(atoms);
    std::vector<c_size> map(atoms.size(), npos);


    for (c_size i = 1; i < copy.size(); ++i)
    {
        int j = i - 1;
        auto k = copy[i];
        while (j >= 0 && copy[j]->getPrecedence() < k->getPrecedence())
        {
            copy[j + 1] = copy[j];
            --j;
        }
        copy[j + 1] = k;
    }

    //not very efficient :(
    for (c_size i = 0; i < map.size(); ++i)
    {
        for (c_size j = 0; j < copy.size(); ++j)
            if (copy[j] == atoms[i])
                map[i] = j;
    }

    atoms = std::move(copy);

    for (c_size i = 0; i < bonds.size(); ++i)
        for (c_size j = 0; j < bonds[i].size(); ++j)
            bonds[i][j].other = map[bonds[i][j].other];

    for (c_size i = 0; i < atoms.size(); ++i)
    {
        if (map[i] == npos || map[i] == i)
            continue;

        c_size p = i;
        auto pB = std::move(bonds[p]);
        do
        {
            auto tempB = std::move(bonds[map[p]]);
            bonds[map[p]] = std::move(pB);
            pB = std::move(tempB);

            const c_size tempP = p;
            p = map[p];
            map[tempP] = npos;
        } while (map[p] != npos);
    }

    for (c_size b = 0; b < bonds.size(); ++b)
    {
        for (c_size i = 1; i < bonds[b].size(); ++i)
        {
            int j = i - 1;
            auto k = bonds[b][i];
            while (j >= 0 &&
                atoms[bonds[b][j].other]->getPrecedence() < atoms[k.other]->getPrecedence())
            {
                bonds[b][j + 1] = bonds[b][j];
                --j;
            }
            bonds[b][j + 1] = k;
        }
    }
}

void MolecularStructure::removeAtom(const c_size idx)
{
    bonds.erase(bonds.begin() + idx);
    delete atoms[idx];
    atoms.erase(atoms.begin() + idx);

    // repair bond indexes
    for (c_size i = 0; i < bonds.size(); ++i)
        for (c_size j = bonds[i].size(); j-- > 0;)
        {
            if (bonds[i][j].other > idx)
                --bonds[i][j].other;
            else if (bonds[i][j].other == idx)
                bonds[i].erase(bonds[i].begin() + j);
        }
}

void MolecularStructure::removeUnnecessaryHydrogens()
{
    for (c_size i = atoms.size(); i-- > 0;)
        if (*atoms[i] == Atom("H") && bonds[i].front().getType() == BondType::SINGLE)
            removeAtom(i);
}

int8_t MolecularStructure::countImpliedHydrogens(const c_size idx) const
{
    const auto d = getDegreeOf(idx);
    const auto v = static_cast<const Atom*>(atoms[idx])->data().getFittingValence(d);

    return v == AtomData::NullValence ? -1 : v - d;
}

int16_t MolecularStructure::countImpliedHydrogens() const
{
    int16_t hCount = 0;
    for (c_size i = 0; i < atoms.size(); ++i)
    {
        const auto h = countImpliedHydrogens(i);
        if (h == -1)
            return -1;

        hCount += h;
    }
    return hCount;
}

uint8_t MolecularStructure::getDegreeOf(const c_size idx) const
{
    uint8_t cnt = 0;
    for (c_size i = 0; i < bonds[idx].size(); ++i)
        cnt += bonds[idx][i].getValence();
    return cnt;
}

const Atom* MolecularStructure::getAtom(const c_size idx) const
{
    return atoms[idx];
}

c_size MolecularStructure::getImpliedHydrogenCount() const
{
    return impliedHydrogenCount;
}

Amount<Unit::GRAM_PER_MOLE> MolecularStructure::getMolarMass() const
{
    Amount<Unit::GRAM> cnt = 0;
    for (c_size i = 0; i < atoms.size(); ++i)
    {
        cnt += atoms[i]->data().weight;
    }
    cnt += Atom("H").data().weight * impliedHydrogenCount;
    return cnt.to<Unit::GRAM_PER_MOLE>(Amount<Unit::MOLE>(1.0));
}

uint8_t MolecularStructure::getDegreesOfFreedom() const
{
    if (atoms.size() == 1) // monoatomics have 3
        return 3;

    if (atoms.size() == 2 || isVirtualHydrogen()) // diatomics have 5
        return 5;

    // TODO: find degreesOfFreedom for other molecules (6 is true for CH4)
    return 6;
}

c_size MolecularStructure::getRadicalAtomsCount() const
{
    c_size cnt = 0;
    c_size i = atoms.size();
    while (i-- > 0 && atoms[i]->isRadical())
        ++cnt;

    return cnt;
}

bool MolecularStructure::isConcrete() const
{
    return isVirtualHydrogen() || atoms.back()->isRadical() == false;
}

bool MolecularStructure::isGeneric() const
{
    return isConcrete() == false;
}

bool MolecularStructure::isOrganic() const
{
    const auto carbon = Atom("C");
    const auto hydrogen = Atom("H");

    for (c_size i = 0; i < atoms.size(); ++i)
    {
        if (carbon == *atoms[i])
        {
            uint8_t cnt = 0;
            for (c_size j = 0; j < bonds[i].size(); ++j)
            {
                if (hydrogen == *atoms[bonds[i][j].other])
                    return true;
                cnt += bonds[i][j].getValence();
            }
            if (carbon.data().getFittingValence(cnt) > cnt)
                return true;
        }
    }

    return false;
}

std::unordered_map<AtomId, c_size> MolecularStructure::getComponentCountMap() const
{
    std::unordered_map<AtomId, c_size> result;
    for (c_size i = 0; i < atoms.size(); ++i)
    {
        if (result.contains(atoms[i]->id))
            ++result[atoms[i]->id];
        else
            result.emplace(std::move(std::make_pair(atoms[i]->id, 1)));
    }
    
    if (impliedHydrogenCount == 0)
        return result;

    const auto hId = Atom("H").id;
    if (result.contains(hId))
        result[hId] += impliedHydrogenCount;
    else
        result.emplace(std::move(std::make_pair(hId, impliedHydrogenCount)));

    return result;
}

bool MolecularStructure::isEmpty() const
{
    return impliedHydrogenCount == 0 && atoms.empty();
}

c_size MolecularStructure::getNonVirtualAtomCount() const
{
    return atoms.size();
}

c_size MolecularStructure::virtualBondCount() const
{
    c_size cnt = 0;
    for (c_size i = 0; i < bonds.size(); ++i)
        cnt += bonds[i].size();
    return cnt;
}

bool MolecularStructure::isCyclic() const
{
    return virtualBondCount() / 2 > atoms.size() - 1;
}

bool MolecularStructure::isConnected() const
{
    if (atoms.size() == 0)
        return true;

    if (virtualBondCount() / 2 < atoms.size() - 1)
        return false;

    std::vector<uint8_t> visited;
    visited.resize(atoms.size(), false);
    std::queue<c_size> queue;

    c_size c = 0;

    while (true)
    {
        visited[c] = true;

        for(c_size i = 0; i < bonds[c].size(); ++i)
            if (visited[bonds[c][i].other] == false) 
                queue.push(bonds[c][i].other);

        if (queue.empty())
            break;

        c = queue.front();
        queue.pop();
    }

    for (c_size i = 0; i < visited.size(); ++i)
        if (visited[i] == false)
            return false;
    return true;
}

bool MolecularStructure::isVirtualHydrogen() const
{
    return atoms.empty() && impliedHydrogenCount == 2;
}

bool MolecularStructure::areAdjacent(const c_size idxA, const c_size idxB) const
{
    for (c_size i = 0; i < bonds[idxA].size(); ++i)
        if (idxB == bonds[idxA][i].other)
            return true;
    return false;
}

void MolecularStructure::rPrint(
    TextBlock& buffer,
    const size_t x,
    const size_t y,
    const c_size c,
    std::vector<uint8_t>& visited,
    const bool printImpliedHydrogens) const
{
    if (x < 1 || y < 1 ||
        x >= buffer.getWidth() - 2 || y >= buffer.getHeight() - 2 ||
        buffer[y][x] != ' ')
        return;

    visited[c] = true;

    const auto& symb = atoms[c]->getSymbol();
    const uint8_t symbSize = symb.size();

    for(uint8_t i = 0; i < symbSize && x + i < buffer[0].size(); ++i)
        buffer[y][x + i] = symb[i];

    for (c_size i = 0; i < bonds[c].size(); ++i)
        if (visited[bonds[c][i].other] == false)
        {
            char vC, hC, d1C, d2C;
            switch (bonds[c][i].getType())
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

            const uint8_t nextSymbSize = atoms[bonds[c][i].other]->getSymbol().size();

            if (buffer[y][x + symbSize + 1] == ' ')
            {
                buffer[y][x + symbSize] = hC;
                rPrint(buffer, x + symbSize + 1, y, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y][x - nextSymbSize - 1] == ' ')
            {
                buffer[y][x - 1] = hC;
                rPrint(buffer, x - nextSymbSize - 1, y, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x] == ' ')
            {
                buffer[y - 1][x] = vC;
                rPrint(buffer, x, y - 2, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x] == ' ')
            {
                buffer[y + 1][x] = vC;
                rPrint(buffer, x, y + 2, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x + symbSize + 1] == ' ')
            {
                buffer[y + 1][x + symbSize] = d1C;
                rPrint(buffer, x + symbSize + 1, y + 2, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x - nextSymbSize - 1] == ' ')
            {
                buffer[y + 1][x - 1] = d2C;
                rPrint(buffer, x - nextSymbSize - 1, y + 2, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x + symbSize + 1] == ' ')
            {
                buffer[y - 1][x + symbSize] = d2C;
                rPrint(buffer, x + symbSize + 1, y - 2, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x - nextSymbSize - 1] == ' ')
            {
                buffer[y - 1][x - 1] = d1C;
                rPrint(buffer, x - nextSymbSize - 1, y - 2, bonds[c][i].other, visited, printImpliedHydrogens);
            }
            else
            {
                Log(this).warn("Incomplete ASCII print.");
                break;
            }
        }

    if (printImpliedHydrogens)
    {
        const auto hCount = countImpliedHydrogens(c);
        for (int8_t i = 0; i < hCount; ++i)
        {
            char vC = '³', hC = 'Ä', d1C = '\\', d2C = '/';

            if (buffer[y][x + symbSize + 1] == ' ')
            {
                buffer[y][x + symbSize] = hC;
                buffer[y][x + symbSize + 1] = 'H';
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
            else if (buffer[y + 2][x + symbSize + 1] == ' ')
            {
                buffer[y + 1][x + symbSize] = d1C;
                buffer[y + 2][x + symbSize + 1] = 'H';
            }
            else if (buffer[y + 2][x - 2] == ' ')
            {
                buffer[y + 1][x - 1] = d2C;
                buffer[y + 2][x - 2] = 'H';
            }
            else if (buffer[y - 2][x + symbSize + 1] == ' ')
            {
                buffer[y - 1][x + symbSize] = d2C;
                buffer[y - 2][x + symbSize + 1] = 'H';
            }
            else if (buffer[y - 2][x - 2] == ' ')
            {
                buffer[y - 1][x - 1] = d1C;
                buffer[y - 2][x - 2] = 'H';
            }
            else
            {
                Log(this).warn("Incomplete ASCII print.");
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
    rPrint(buffer, buffer.getWidth() / 4, buffer.getHeight() / 2, 0, visited, !isOrganic());

    buffer.trim();
    return buffer.toString();
}

void MolecularStructure::clear()
{
    while (atoms.empty() == false)
    {
        bonds[atoms.size() - 1].clear();
        delete atoms.back();
        atoms.pop_back();
    }
    impliedHydrogenCount = 0;
}

MolecularStructure MolecularStructure::createCopy() const
{
    return MolecularStructure(*this);
}

bool MolecularStructure::areMatching(
    const c_size idxA, const MolecularStructure& a,
    const c_size idxB, const MolecularStructure& b,
    const bool escapeRadicalTypes)
{
    if (a.bonds[idxA].size() != b.bonds[idxB].size())
        return false;

    return escapeRadicalTypes ?
        b.atoms[idxB]->matches(*a.atoms[idxA]) :
        b.atoms[idxB]->equals(*a.atoms[idxA]);
}

bool MolecularStructure::areMatching(
    const Bond& nextA, const MolecularStructure& a,
    const Bond& nextB, const MolecularStructure& b,
    const bool escapeRadicalTypes)
{
    if (nextA.getType() != nextB.getType())
        return false;

    // escape radical types
    if (escapeRadicalTypes == true && b.atoms[nextB.other]->isRadical())
        return b.atoms[nextB.other]->matches(*a.atoms[nextA.other]);

    if (areMatching(nextA.other, a, nextB.other, b, escapeRadicalTypes) == false)
        return false;

    // test to see both have the same types of bonds
    std::array<int8_t, 5> counts{ 0 };
    for (c_size i = 0; i < a.bonds[nextA.other].size(); ++i)
    {
        ++counts[a.bonds[nextA.other][i].getValence()];
        --counts[b.bonds[nextB.other][i].getValence()];
    }
    for (uint8_t i = 0; i < counts.size(); ++i)
        if (counts[i] != 0)
            return false;

    return true;
}

uint8_t MolecularStructure::getBondSimilarity(
    const c_size idxA, const MolecularStructure& a,
    const c_size idxB, const MolecularStructure& b)
{
    uint8_t score = 255;
    uint8_t scorePerBond = a.bonds[idxA].size() / 255;
    std::array<int8_t, 5> counts{ 0 };

    for (c_size i = 0; i < a.bonds[idxA].size(); ++i)
        ++counts[a.bonds[idxA][i].getValence()];
    for (c_size i = 0; i < b.bonds[idxB].size(); ++i)
        --counts[b.bonds[idxB][i].getValence()];

    for (uint8_t i = 0; i < counts.size(); ++i)
        score -= counts[i] * scorePerBond;

    return score;
}

uint8_t MolecularStructure::maximalSimilarity(
    const Bond& nextA, const MolecularStructure& a,
    const Bond& nextB, const MolecularStructure& b)
{
    if (nextA.getType() != nextB.getType())
        return 0;

    if(b.atoms[nextB.other]->equals(*a.atoms[nextA.other]) == false)
        return 0;

    return getBondSimilarity(nextA.other, a, nextB.other, b);
}

bool MolecularStructure::DFSCompare(
    c_size idxA, const MolecularStructure& a,
    c_size idxB, const MolecularStructure& b,
    std::vector<uint8_t>& visitedB,
    std::unordered_map<c_size, c_size>& mapping,
    bool escapeRadicalTypes)
{
    mapping.emplace(std::move(std::make_pair(idxA, idxB)));
    visitedB[idxB] = true;

    for (c_size i = 0; i < b.bonds[idxB].size(); ++i)
    {
        const Bond& next = b.bonds[idxB][i];
        if (visitedB[next.other])
        {
            // here we could mark the nodes contained in cycles
            continue;
        }

        bool matchFound = false;
        for (c_size j = 0; j < a.bonds[idxA].size(); ++j)
        {
            if (mapping.contains(a.bonds[idxA][j].other) ||  // do not reuse already mapped nodes
                areMatching(a.bonds[idxA][j], a, next, b, escapeRadicalTypes) == false)
                continue;

            if (DFSCompare(a.bonds[idxA][j].other, a, next.other, b, visitedB, mapping, escapeRadicalTypes))
            {
                matchFound = true;
                break;
            }
            
            // revert wrong branch
            visitedB[next.other] = false;
            mapping.erase(a.bonds[idxA][j].other);
        }

        if (matchFound == false)
        {
            return false;
        }
    }

    return true;
}

std::pair<std::unordered_map<c_size, c_size>, uint8_t> MolecularStructure::DFSMaximal(
    c_size idxA, const MolecularStructure& a,
    std::unordered_set<c_size>& mappedA,
    c_size idxB, const MolecularStructure& b,
    std::unordered_set<c_size>& mappedB)
{
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> newMap;
    newMap.first.emplace(std::move(std::make_pair(idxA, idxB)));
    mappedA.emplace(idxA);
    mappedB.emplace(idxB);

    for (c_size i = 0; i < b.bonds[idxB].size(); ++i)
    {
        const Bond& next = b.bonds[idxB][i];
        if (mappedB.contains(next.other))
        {
            continue;
        }

        // only the largest mapping is added into the final but states need to be copied
        std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMapping;
        std::unordered_set<c_size> maxMappedA;
        std::unordered_set<c_size> maxMappedB;
        for (c_size j = 0; j < a.bonds[idxA].size(); ++j)
        {
            if (mappedA.contains(a.bonds[idxA][j].other)) // do not reuse already mapped nodes
                continue;

            const auto score = maximalSimilarity(a.bonds[idxA][j], a, next, b);
            if (score == 0)
                continue;

            // reversing bad branches isn't possible here, so copies are needed :(
            auto mappedACopy = mappedA;
            auto mappedBCopy = mappedB;
            auto subMap = DFSMaximal(a.bonds[idxA][j].other, a, mappedACopy, next.other, b, mappedBCopy);
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

bool MolecularStructure::checkConnectivity(
    const MolecularStructure& target,
    const MolecularStructure& pattern,
    const std::unordered_map<c_size, c_size>& mapping)
{
    for (auto const& m : mapping)
    {
        for (auto const& n : mapping)
            if (pattern.areAdjacent(m.second, n.second) &&
                target.areAdjacent(m.first, n.first) == false)
            {
                return false;
            }
    }
    return true;
}

std::unordered_map<c_size, c_size> MolecularStructure::mapTo(const MolecularStructure& pattern, bool escapeRadicalTypes) const
{
    if (pattern.getNonVirtualAtomCount() > this->getNonVirtualAtomCount() || pattern.getNonVirtualAtomCount() == 0)
        return std::unordered_map<c_size, c_size>();

    // should start with a non radical type, canonicalization assures this
    const c_size pStart = 0;
    for (c_size i = 0; i < this->getNonVirtualAtomCount(); ++i)
    {
        if (areMatching(i, *this, pStart, pattern, escapeRadicalTypes))
        {
            std::vector<uint8_t> visited(pattern.getNonVirtualAtomCount(), false);
            std::unordered_map<c_size, c_size> mapping;

            if (DFSCompare(i, *this, pStart, pattern, visited, mapping, escapeRadicalTypes) == false)
                continue;

            if (pattern.isCyclic() == false)
                return mapping;

            // re-verifying connectivity is necessary for cycles
            if(checkConnectivity(*this, pattern, mapping))
                return mapping;
        }
    }
    return std::unordered_map<c_size, c_size>();
}

std::pair<std::unordered_map<c_size, c_size>, uint8_t> MolecularStructure::maximalMapTo(
    const MolecularStructure& pattern,
    const std::unordered_set<c_size>& targetIgnore,
    const std::unordered_set<c_size>& patternIgnore) const
{
    if (pattern.getNonVirtualAtomCount() == 0 || this->getNonVirtualAtomCount() == 0)
        return std::pair<std::unordered_map<c_size, c_size>, uint8_t>();

    // find atom that matches in both target and pattern
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMapping;
    uint8_t maxScore = 0;
    for (c_size i = 0; i < this->atoms.size(); ++i)
    {
        if (targetIgnore.contains(i))
            continue;

        // skip some comparisons
        if (maxMapping.first.contains(i))
            continue;

        for (c_size j = 0; j < pattern.atoms.size(); ++j)
        {
            if (patternIgnore.contains(j))
                continue;

            if(pattern.atoms[j]->equals(*this->atoms[i]) == false)
                continue;

            const auto score = getBondSimilarity(i, *this, j, pattern);
            std::unordered_set<c_size> mappedA(targetIgnore), mappedB(patternIgnore);
            auto map = DFSMaximal(i, *this, mappedA, j, pattern, mappedB);

            // picks largest mapping, then best 2nd comp. score, then best 1st comp. score
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


void MolecularStructure::recountImpliedHydrogens()
{
    if(!isVirtualHydrogen())
        impliedHydrogenCount = countImpliedHydrogens();
}

void MolecularStructure::copyBranch(
    MolecularStructure& destination,
    const MolecularStructure& source,
    const c_size sourceIdx,
    std::unordered_map<c_size, c_size>& sdMapping,
    bool canonicalize,
    const std::unordered_set<c_size>& sourceIgnore)
{
    // overwrites first matching radical atom
    if (destination.atoms[sdMapping[sourceIdx]]->isRadical())
    {
        delete destination.atoms[sdMapping[sourceIdx]];
        destination.atoms[sdMapping[sourceIdx]] = source.atoms[sourceIdx]->clone();
    }

    std::queue<c_size> queue;
    for (c_size i = 0; i < source.bonds[sourceIdx].size(); ++i)
        if(sdMapping.contains(source.bonds[sourceIdx][i].other) == false && sourceIgnore.contains(source.bonds[sourceIdx][i].other) == false)
            queue.push(source.bonds[sourceIdx][i].other);

    if (queue.empty())
        return;

    while (queue.size()) 
    {
        const auto c = queue.front();
        queue.pop();

        // add current node
        sdMapping.insert(std::make_pair(c, destination.atoms.size()));
        destination.atoms.emplace_back(source.atoms[c]->clone());
        destination.bonds.emplace_back(std::vector<Bond>());

        // add bonds to existing nodes and queue non-existing nodes
        for (c_size i = 0; i < source.bonds[c].size(); ++i)
        {
            if (sdMapping.contains(source.bonds[c][i].other))
            {
                destination.bonds.back().emplace_back(Bond(
                    sdMapping.at(source.bonds[c][i].other),
                    source.bonds[c][i].getType())
                );

                destination.bonds[sdMapping.at(source.bonds[c][i].other)].emplace_back(Bond(
                    destination.atoms.size() - 1,
                    source.bonds[c][i].getType())
                );
            }
            else if(sourceIgnore.contains(source.bonds[c][i].other) == false)
            {
                queue.push(source.bonds[c][i].other);
            }
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
    std::unordered_map<c_size, c_size>& ipMap,
    bool canonicalize)
{
    MolecularStructure result(pattern);

    for (auto const& p : ipMap)
    {
        copyBranch(result, instance, p.first, ipMap, false);
    }

    if (canonicalize)
    {
        result.canonicalize();
        result.recountImpliedHydrogens();
        ipMap.clear();
    }

    return result;
}

bool MolecularStructure::operator==(const MolecularStructure& other) const
{
    if (this->getNonVirtualAtomCount() != other.getNonVirtualAtomCount() || this->impliedHydrogenCount != other.impliedHydrogenCount)
        return false;

    const auto mapping = this->mapTo(other, false);
    return mapping.size() == this->getNonVirtualAtomCount();
}

bool MolecularStructure::operator!=(const MolecularStructure& other) const
{
    if (this->getNonVirtualAtomCount() != other.getNonVirtualAtomCount() || this->impliedHydrogenCount != other.impliedHydrogenCount)
        return true;

    const auto mapping = this->mapTo(other, false);
    return mapping.size() != this->getNonVirtualAtomCount();
}

bool MolecularStructure::operator==(const std::string& other) const
{
    return *this == MolecularStructure(other);
}

bool MolecularStructure::operator!=(const std::string& other) const
{
    return *this != MolecularStructure(other);
}

void MolecularStructure::insertCycleHeads(
    std::string& smiles,
    const std::vector<size_t>& insertPositions,
    const std::map<c_size, uint8_t>& cycleHeads
)
{
    for (auto h = cycleHeads.crbegin(); h != cycleHeads.crend(); ++h)
        smiles.insert(insertPositions[h->first], std::to_string(h->second));
}

std::string MolecularStructure::rToSMILES(
    c_size c, c_size prev,
    std::vector<size_t>& insertPositions,
    uint8_t& cycleCount,
    std::map<c_size, uint8_t>& cycleHeads,
    const size_t insertOffset) const
{
    std::string smiles;
    while (true)
    {
        smiles += atoms[c]->getSMILES();
        insertPositions[c] = insertOffset + smiles.size();

        if (bonds[c].size() <= 1)  // C-C-C     end of chain, only neighbour == prev
            break;                 //     ^

        if (bonds[c].size() == 2)   // C-C-C
        {                           //   ^
            const Bond& bondToNext = bonds[c].front().other != prev ?
                bonds[c].front() :
                bonds[c].back();

            if (insertPositions[bondToNext.other] != std::string::npos)  // cycle end
            {
                const auto [_, newHead] = cycleHeads.emplace(std::make_pair(bondToNext.other, cycleCount));
                smiles += std::to_string(cycleCount);

                if (newHead)
                    ++cycleCount;

                break;
            }
            else
            {
                smiles += bondToNext.getSMILES();
                prev = c;
                c = bondToNext.other;
            }

            continue;
        }
                                                                //   C
        for (c_size i = 0; i < bonds[c].size() - 1; ++i)        //   |
        {                                                       // C-C-C
            if (bonds[c][i].other == prev)                      //   ^
                continue;

            if (insertPositions[bonds[c][i].other] != std::string::npos)     // cycle end
            {
                if (smiles.back() == ')')
                    continue;

                const auto [_, newHead] = cycleHeads.emplace(std::make_pair(bonds[c][i].other, cycleCount));
                smiles += std::to_string(cycleCount);

                if (newHead)
                    ++cycleCount;
            }
            else
            {
                if (bonds[c].size() >= 4)
                {
                    const auto temp = rToSMILES(bonds[c][i].other, c, insertPositions, cycleCount, cycleHeads, insertOffset + smiles.size() + 1);
                    smiles += '(' + bonds[c][i].getSMILES() + temp + ')';
                }
                else
                {
                    const auto before = cycleCount;
                    const auto temp = rToSMILES(bonds[c][i].other, c, insertPositions, cycleCount, cycleHeads, insertOffset + smiles.size());
                    if (cycleCount == before || bonds[c].size() >= 4)
                        smiles += '(' + bonds[c][i].getSMILES() + temp + ')';
                    else
                        smiles += bonds[c][i].getSMILES() + temp;
                }
            }
        }

        if (insertPositions[bonds[c].back().other] != std::string::npos)  // cycle end
        {
            break;
        }
        else
        {
            smiles += bonds[c].back().getSMILES();
            prev = c;
            c = bonds[c].back().other;
        }
    }
    return smiles;
}

std::string MolecularStructure::toSMILES() const
{
    if (isVirtualHydrogen())
        return "HH";

    if (atoms.empty())
        return "";

    std::vector<size_t> insertPositions(atoms.size(), std::string::npos);
    uint8_t cycleCount = 1;
    std::map<c_size, uint8_t> cycleHeads;
    std::string smiles;

    smiles += atoms.front()->getSMILES();
    insertPositions.front() = smiles.size();

    if (bonds.front().empty())
        return smiles;

    if (bonds.front().size() == 1)
    {
        smiles += bonds.front().front().getSMILES();
        smiles += rToSMILES(bonds.front().front().other, 0, insertPositions, cycleCount, cycleHeads, smiles.size());
        insertCycleHeads(smiles, insertPositions, cycleHeads);
        return smiles;
    }

    for (c_size i = 0; i < bonds.front().size() - 1; ++i)
    {        
        const auto before = cycleCount;
        const auto temp = rToSMILES(bonds.front()[i].other, 0, insertPositions, cycleCount, cycleHeads, smiles.size());
        if(cycleCount == before)
            smiles += '(' + bonds.front()[i].getSMILES() + temp + ')';
        else
            smiles += bonds.front()[i].getSMILES() + temp;
    }

    if (insertPositions[bonds.front().back().other] != std::string::npos)  // cycle end
    {
        //const auto [_, newHead] = cycleHeads.emplace(std::make_pair(bonds.front().back().other, cycleCount));
        //cycleHeads.emplace(std::make_pair(0, cycleCount));
        //smiles += std::to_string(cycleCount);

        insertCycleHeads(smiles, insertPositions, cycleHeads);
        return smiles;
    }
    else
    {
        smiles += bonds.front().back().getSMILES();
        const auto temp = rToSMILES(bonds.front().back().other, 0, insertPositions, cycleCount, cycleHeads, smiles.size());
        smiles += bonds.front().back().getSMILES() + temp;
    }

    insertCycleHeads(smiles, insertPositions, cycleHeads);
    return smiles;
}

std::string MolecularStructure::serialize() const
{
    if (isVirtualHydrogen())
        return "h";

    std::string result;
    for (c_size i = 0; i < atoms.size(); ++i)
        result += atoms[i]->data().getBinaryId() + ';';
    result.back() = '_';
    for (c_size i = 0; i < atoms.size(); ++i)
    {
        for (c_size j = 0; j < bonds[i].size(); ++j)
            result += bonds[i][j].getSMILES() + std::to_string(bonds[i][j].other) + ';';
        result.back() = '_';
    }
    result.pop_back();
    return result;
}

bool MolecularStructure::deserialize(const std::string& str)
{
    clear();
    
    if (str == "h") // pure virtual hydrogen
    {
        impliedHydrogenCount = 2;
        return true;
    }

    const auto tokens = Utils::split(str, '_');
    if (tokens.empty())
    {
        clear();
        return false;
    }

    auto comps = Utils::split(tokens[0], ';');
    for (c_size i = 0; i < comps.size(); ++i)
    {
        const auto id = Def::parse<unsigned int>(comps[i]);
        if (id.has_value() == false || Atom::isDefined(static_cast<AtomId>(*id)) == false)
        {
            clear();
            return false;
        }

        atoms.emplace_back(AtomFactory::get(static_cast<AtomId>(*id)));
    }

    for (c_size i = 1; i < tokens.size(); ++i)
    {
        comps = Utils::split(tokens[i], ';');
        bonds.emplace_back(std::vector<Bond>());
        for (c_size j = 0; j < comps.size(); ++j)
        {
            if (comps[j].empty())
            {
                clear();
                return false;
            }

            if (std::isdigit(comps[j][0]))
            {
                const auto id = Def::parse<unsigned int>(comps[j]);
                if (id.has_value() == false)
                {
                    clear();
                    return false;
                }

                bonds[i - 1].emplace_back(Bond(*id, BondType::SINGLE));
            }
            else
            {
                const auto bType = Bond::fromSMILES(comps[j][0]);
                if (bType == BondType::NONE)
                {
                    clear();
                    return false;
                }

                const auto id = Def::parse<unsigned int>(comps[j].substr(1));
                if (id.has_value() == false)
                {
                    clear();
                    return false;
                }

                  bonds[i - 1].emplace_back(Bond(*id, bType));
            }
        }
    }

    if (isConnected() == false)
    {
        clear();
        return false;
    }

    const auto hCount = countImpliedHydrogens();
    if (hCount == -1)
    {
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;
    return true;
}
