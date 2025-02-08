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
    if (not loadFromSMILES(smiles))
        return;
    canonicalize();
}

MolecularStructure::MolecularStructure(const MolecularStructure& other) noexcept :
    impliedHydrogenCount(other.impliedHydrogenCount)
{
    this->atoms.reserve(other.atoms.size());
    for (const auto& otherAtom : other.atoms)
        this->atoms.emplace_back(std::make_unique<BondedAtom>(*otherAtom));

    // Bond other atom references must be updated
    for (auto& a : this->atoms)
        for (auto& b : a->bonds)
            b.setOther(*this->atoms[b.getOther().index]);
}

void MolecularStructure::addBond(BondedAtom& from, BondedAtom& to, const BondType bondType)
{
    from.bonds.emplace_back(Bond(to, bondType));
    to.bonds.emplace_back(Bond(from, bondType));
}

bool MolecularStructure::addBondChecked(BondedAtom& from, BondedAtom& to, const BondType bondType)
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
                atoms.emplace_back(std::make_unique<BondedAtom>(AtomFactory::get(symbol)));
            }
            else if (Symbol symbol(smiles.substr(i, 2)); Atom::isDefined(symbol))
            {
                atoms.emplace_back(std::make_unique<BondedAtom>(AtomFactory::get(symbol)));
                ++i;
            }
            else
            {
                Log(this).error("Atomic symbol '{0}' at index {1} is undefined.", smiles[i], i);
                clear();
                return false;
            }

            if (prev != npos)
            {
                addBond(*atoms[prev], *atoms.back(), bondType);
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
               Log(this).error("Unpaired ')' found at index {0}.", i);
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
                Log(this).error("Unpaired '[' found at index {0}.", i);
                clear();
                return false;
            }

            const Symbol& symbol(smiles.substr(i + 1, t - i - 1));
            if (Atom::isDefined(symbol) == false)
            {
                Log(this).error("Atomic symbol '{0}' at index {1} is undefined.", symbol.getString(), i);
                clear();
                return false;
            }
            
            atoms.emplace_back(std::make_unique<BondedAtom>(AtomFactory::get(symbol)));

            if (prev != npos)
            {
                addBond(*atoms[prev], *atoms.back(), bondType);
                bondType = BondType::SINGLE;
            }

            prev = atoms.size() - 1;
            i = t;
            continue;
        }

        if (smiles[i] == '%')
        {
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

            if (not addBondChecked(*atoms[prev], *atoms[rings[label]], bondType))
            {
                Log(this).error("Illegal ring-closing bond with label: '{0}'.", label);
                clear();
                return false;
            }

            bondType = BondType::SINGLE;
            continue;
        }

        if (isdigit(smiles[i]))
        {
            const uint8_t label = smiles[i] - '0';
            if (not rings.contains(label))
            {
                rings.emplace(label, atoms.size() - 1);
                continue;
            }
            
            if (not addBondChecked(*atoms[prev], *atoms[rings[label]], bondType))
            {
                Log(this).error("Illegal ring-closing bond with label: '{0}'.", label);
                clear();
                return false;
            }

            bondType = BondType::SINGLE;
            continue;
        }

        Log(this).error("Unidentified symbol '{0}' found at index {1}.", smiles[i], i);
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
        Log(this).error("Valence of an atom was exceeded in SMILES: '{0}'.", smiles);
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;

    return true;
}

void MolecularStructure::canonicalize()
{
    if (atoms.size() == 0)
        return;

    if(atoms.size() == 1)
    {
        atoms.front()->index = 0;
        return;
    }

    std::sort(atoms.begin(), atoms.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs->atom->getPrecedence() > rhs->atom->getPrecedence();
        });

    for (c_size i = 0; i < atoms.size(); ++i)
        atoms[i]->index = i;

    // Sort bonds by the procedence of the other atom and valence
    for (auto& a : atoms)
    {
        std::sort(a->bonds.begin(), a->bonds.end(), [this](const auto& lhs, const auto& rhs)
            {
                const auto lhsPrecedence = lhs.getOther().atom->getPrecedence();
                const auto rhsPrecedence = rhs.getOther().atom->getPrecedence();

                return
                    lhsPrecedence > rhsPrecedence ? true :
                    lhsPrecedence < rhsPrecedence ? false:
                    lhs.getValence() > rhs.getValence();
            });
    }
}

void MolecularStructure::removeAtom(const c_size idx)
{
    const auto it = atoms.begin() + idx;

    // Remove all bonds leading to the removed atom
    for (auto& a : atoms)
    {
        for (c_size j = a->bonds.size(); j-- > 0;)
        {
            if ((*it)->atom.get() != a->bonds[j].getOther().atom.get())
                continue;

            a->bonds.erase(a->bonds.begin() + j);
            break;
        }
    }

    atoms.erase(it);
}

void MolecularStructure::removeUnnecessaryHydrogens()
{
    // Strip non-stereochemical hydrogens
    static const auto hydrogen = Atom("H");
    for (c_size i = atoms.size(); i-- > 0;)
        if (*atoms[i]->atom == hydrogen && atoms[i]->bonds.front().getType() == BondType::SINGLE)
            removeAtom(i);
}

int8_t MolecularStructure::countImpliedHydrogens(const BondedAtom& atom) const
{
    const auto d = getDegreeOf(atom);
    const auto v = atom.atom->getData().getFittingValence(d);

    return v == AtomData::NullValence ? -1 : v - d;
}

int16_t MolecularStructure::countImpliedHydrogens() const
{
    int16_t hCount = 0;
    for (const auto& a : atoms)
    {
        const auto h = countImpliedHydrogens(*a);
        if (h == -1)
            return -1;

        hCount += h;
    }
    return hCount;
}

uint8_t MolecularStructure::getDegreeOf(const BondedAtom& atom) const
{
    uint8_t cnt = 0;
    for (c_size i = 0; i < atom.bonds.size(); ++i)
        cnt += atom.bonds[i].getValence();
    return cnt;
}

const Atom& MolecularStructure::getAtom(const c_size idx) const
{
    return *atoms[idx]->atom.get();
}

c_size MolecularStructure::getImpliedHydrogenCount() const
{
    return impliedHydrogenCount;
}

Amount<Unit::GRAM_PER_MOLE> MolecularStructure::getMolarMass() const
{
    Amount<Unit::GRAM> cnt = 0;
    for (c_size i = 0; i < atoms.size(); ++i)
        cnt += atoms[i]->atom->getData().weight;

    cnt += Atom("H").getData().weight * impliedHydrogenCount;
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
    while (i-- > 0 && atoms[i]->atom->isRadical())
        ++cnt;

    return cnt;
}

bool MolecularStructure::isConcrete() const
{
    return isVirtualHydrogen() || atoms.back()->atom->isRadical() == false;
}

bool MolecularStructure::isGeneric() const
{
    return isConcrete() == false;
}

bool MolecularStructure::isOrganic() const
{
    static const auto carbon = Atom("C");
    static const auto hydrogen = Atom("H");

    for (c_size i = 0; i < atoms.size(); ++i)
    {
        if (carbon == *atoms[i]->atom)
        {
            uint8_t cnt = 0;
            for (c_size j = 0; j < atoms[i]->bonds.size(); ++j)
            {
                if (hydrogen == *atoms[i]->bonds[j].getOther().atom)
                    return true;
                cnt += atoms[i]->bonds[j].getValence();
            }
            if (carbon.getData().getFittingValence(cnt) > cnt)
                return true;
        }
    }

    return false;
}

std::unordered_map<Symbol, c_size> MolecularStructure::getComponentCountMap() const
{
    std::unordered_map<Symbol, c_size> result;
    for (c_size i = 0; i < atoms.size(); ++i)
    {
        if (result.contains(atoms[i]->atom->getData().symbol))
            ++result[atoms[i]->atom->getData().symbol];
        else
            result.emplace(atoms[i]->atom->getData().symbol, 1);
    }
    
    if (impliedHydrogenCount == 0)
        return result;

    const auto hId = Atom("H").getData().symbol;
    if (result.contains(hId))
        result[hId] += impliedHydrogenCount;
    else
        result.emplace(hId, impliedHydrogenCount);

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
    for (c_size i = 0; i < atoms.size(); ++i)
        cnt += atoms[i]->bonds.size();
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

        for(c_size i = 0; i < atoms[c]->bonds.size(); ++i)
            if (not visited[atoms[c]->bonds[i].getOther().index])
                queue.push(atoms[c]->bonds[i].getOther().index);

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
    const auto& atomA = atoms[idxA];
    for (const auto& b : atoms[idxB]->bonds)
        if (b.getOther().isSame(*atomA))
            return true;
    return false;
}

void MolecularStructure::rPrint(
    TextBlock& buffer,
    const size_t x,
    const size_t y,
    const BondedAtom& current,
    std::vector<uint8_t>& visited,
    const bool printImpliedHydrogens) const
{
    if (x < 1 || y < 1 ||
        x >= buffer.getWidth() - 2 || y >= buffer.getHeight() - 2 ||
        buffer[y][x] != ' ')
        return;

    visited[current.index] = true;

    const auto& symb = current.atom->getSymbol();
    const uint8_t symbSize = symb.size();

    for(uint8_t i = 0; i < symbSize && x + i < buffer[0].size(); ++i)
        buffer[y][x + i] = symb[i];

    for (c_size i = 0; i < current.bonds.size(); ++i)
    {
        const auto& other = current.bonds[i].getOther();
        if (visited[other.index] == false)
        {
            char vC, hC, d1C, d2C;
            switch (current.bonds[i].getType())
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

            const uint8_t nextSymbSize = other.atom->getSymbol().size();

            if (buffer[y][x + symbSize + 1] == ' ')
            {
                buffer[y][x + symbSize] = hC;
                rPrint(buffer, x + symbSize + 1, y, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y][x - nextSymbSize - 1] == ' ')
            {
                buffer[y][x - 1] = hC;
                rPrint(buffer, x - nextSymbSize - 1, y, other, visited, printImpliedHydrogens);
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
            else if (buffer[y + 2][x + symbSize + 1] == ' ')
            {
                buffer[y + 1][x + symbSize] = d1C;
                rPrint(buffer, x + symbSize + 1, y + 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y + 2][x - nextSymbSize - 1] == ' ')
            {
                buffer[y + 1][x - 1] = d2C;
                rPrint(buffer, x - nextSymbSize - 1, y + 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x + symbSize + 1] == ' ')
            {
                buffer[y - 1][x + symbSize] = d2C;
                rPrint(buffer, x + symbSize + 1, y - 2, other, visited, printImpliedHydrogens);
            }
            else if (buffer[y - 2][x - nextSymbSize - 1] == ' ')
            {
                buffer[y - 1][x - 1] = d1C;
                rPrint(buffer, x - nextSymbSize - 1, y - 2, other, visited, printImpliedHydrogens);
            }
            else
            {
                Log(this).warn("Incomplete ASCII print.");
                break;
            }
        }
    }

    if (printImpliedHydrogens)
    {
        const auto hCount = countImpliedHydrogens(current);
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
    rPrint(buffer, buffer.getWidth() / 4, buffer.getHeight() / 2, *atoms.front(), visited, !isOrganic());

    buffer.trim();
    return buffer.toString();
}

void MolecularStructure::clear()
{
    atoms.clear();
    impliedHydrogenCount = 0;
}

MolecularStructure MolecularStructure::createCopy() const
{
    return MolecularStructure(*this);
}

bool MolecularStructure::areMatching(
    const BondedAtom& a, const BondedAtom& b,
    const bool escapeRadicalTypes)
{
    if (a.bonds.size() != b.bonds.size())
        return false;

    return escapeRadicalTypes ?
        b.atom->matches(*a.atom) :
        b.atom->equals(*a.atom);
}

bool MolecularStructure::areMatching(
    const Bond& nextA, const Bond& nextB,
    const bool escapeRadicalTypes)
{
    if (nextA.getType() != nextB.getType())
        return false;

    // escape radical types
    if (escapeRadicalTypes == true && nextB.getOther().atom->isRadical())
        return nextB.getOther().atom->matches(*nextA.getOther().atom);

    if (areMatching(nextA.getOther(), nextB.getOther(), escapeRadicalTypes) == false)
        return false;

    // test to see both have the same types of bonds
    // TODO: could be more efficient with canonicalization
    std::array<int8_t, 5> counts{ 0 };
    for (c_size i = 0; i < nextA.getOther().bonds.size(); ++i)
    {
        ++counts[nextA.getOther().bonds[i].getValence()];
        --counts[nextB.getOther().bonds[i].getValence()];
    }
    for (uint8_t i = 0; i < counts.size(); ++i)
        if (counts[i] != 0)
            return false;

    return true;
}

uint8_t MolecularStructure::getBondSimilarity(const BondedAtom& a, const BondedAtom& b)
{
    uint8_t score = 255;
    std::array<int8_t, 5> counts{ 0 };

    for (const auto& bondA : a.bonds)
        ++counts[bondA.getValence()];
    for (const auto& bondB : b.bonds)
        --counts[bondB.getValence()];

    const uint8_t scorePerBond = a.bonds.size() / 255;
    for (uint8_t i = 0; i < counts.size(); ++i)
        score -= counts[i] * scorePerBond;

    return score;
}

uint8_t MolecularStructure::maximalSimilarity(const Bond& nextA, const Bond& nextB)
{
    if (nextA.getType() != nextB.getType())
        return 0;

    if(not nextB.getOther().atom->equals(*nextA.getOther().atom))
        return 0;

    return getBondSimilarity(nextA.getOther(), nextB.getOther());
}

bool MolecularStructure::DFSCompare(
    const BondedAtom& a, const BondedAtom& b,
    std::vector<uint8_t>& visitedB,
    std::unordered_map<c_size, c_size>& mapping,
    bool escapeRadicalTypes)
{
    mapping.emplace(a.index, b.index);
    visitedB[b.index] = true;

    for (c_size i = 0; i < b.bonds.size(); ++i)
    {
        const Bond& next = b.bonds[i];
        if (visitedB[next.getOther().index])
            continue;

        bool matchFound = false;
        for (c_size j = 0; j < a.bonds.size(); ++j)
        {
            if (mapping.contains(a.bonds[j].getOther().index) ||  // do not reuse already mapped nodes
                not areMatching(a.bonds[j], next, escapeRadicalTypes))
                continue;

            if (DFSCompare(a.bonds[j].getOther(), next.getOther(), visitedB, mapping, escapeRadicalTypes))
            {
                matchFound = true;
                break;
            }
            
            // revert wrong branch
            visitedB[next.getOther().index] = false;
            mapping.erase(a.bonds[j].getOther().index);
        }

        if (matchFound == false)
        {
            return false;
        }
    }

    return true;
}

std::pair<std::unordered_map<c_size, c_size>, uint8_t> MolecularStructure::DFSMaximal(
    const BondedAtom& a, std::unordered_set<c_size>& mappedA,
    const BondedAtom& b, std::unordered_set<c_size>& mappedB)
{
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> newMap;
    newMap.first.emplace(a.index, b.index);
    mappedA.emplace(a.index);
    mappedB.emplace(b.index);

    for (c_size i = 0; i < b.bonds.size(); ++i)
    {
        const Bond& next = b.bonds[i];
        if (mappedB.contains(next.getOther().index))
            continue;

        // only the largest mapping is added into the final but states need to be copied
        std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMapping;
        std::unordered_set<c_size> maxMappedA;
        std::unordered_set<c_size> maxMappedB;
        for (c_size j = 0; j < a.bonds.size(); ++j)
        {
            if (mappedA.contains(a.bonds[j].getOther().index)) // do not reuse already mapped nodes
                continue;

            const auto score = maximalSimilarity(a.bonds[j], next);
            if (score == 0)
                continue;

            // reversing bad branches isn't possible here, so copies are needed :(
            auto mappedACopy = mappedA;
            auto mappedBCopy = mappedB;
            auto subMap = DFSMaximal(a.bonds[j].getOther(), mappedACopy, next.getOther(), mappedBCopy);
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

    const c_size pStart = 0;
    for (const auto& a : this->atoms)
    {
        // Should start with a non radical type from pattern.
        // Canonicalization assures that if such atom exists, it is the first.
        if (areMatching(*a, *pattern.atoms.front(), escapeRadicalTypes))
        {
            std::vector<uint8_t> visited(pattern.getNonVirtualAtomCount(), false);
            std::unordered_map<c_size, c_size> mapping;

            if (DFSCompare(*a, *pattern.atoms.front(), visited, mapping, escapeRadicalTypes) == false)
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

            if(pattern.atoms[j]->atom->equals(*this->atoms[i]->atom) == false)
                continue;

            const auto score = getBondSimilarity(*this->atoms[i], *pattern.atoms[j]);
            std::unordered_set<c_size> mappedA(targetIgnore), mappedB(patternIgnore);
            auto map = DFSMaximal(*this->atoms[i], mappedA, *pattern.atoms[j], mappedB);

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
    const auto& sourceAtom = source.atoms[sourceIdx];

    // overwrites first matching radical atom
    if (destination.atoms[sdMapping[sourceIdx]]->atom->isRadical())
        destination.atoms[sdMapping[sourceIdx]]->replace(sourceAtom->atom->clone());

    std::queue<c_size> queue;
    for (c_size i = 0; i < sourceAtom->bonds.size(); ++i)
        if(sdMapping.contains(sourceAtom->bonds[i].getOther().index) == false &&
            sourceIgnore.contains(sourceAtom->bonds[i].getOther().index) == false)
            queue.push(sourceAtom->bonds[i].getOther().index);

    if (queue.empty())
        return;

    while (queue.size()) 
    {
        const auto c = queue.front();
        queue.pop();

        // add current node
        sdMapping.emplace(c, destination.atoms.size());
        destination.atoms.emplace_back(std::make_unique<BondedAtom>(source.atoms[c]->atom->clone(), destination.atoms.size()));

        // add bonds to existing nodes and queue non-existing nodes
        for (const auto& bond : source.atoms[c]->bonds)
        {
            if (const auto sdIt = sdMapping.find(bond.getOther().index); sdIt != sdMapping.end())
            {
                addBond(*destination.atoms.back(), *destination.atoms[sdIt->second], bond.getType());
                continue;
            }

            if (not sourceIgnore.contains(bond.getOther().index))
                queue.push(bond.getOther().index);
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
        // TODO: it seems problematic that ipMap is modified, possibly invalidating iterators
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
        ) noexcept :
            idxA(idxA),
            idxB(idxB),
            tag(tag)
        {}
        CycleClosure(const CycleClosure&) = default;
    };

    class ClosureComparator
    {
    public:
        bool operator()(const CycleClosure& lhs, const CycleClosure& rhs) const
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
    };

private:
    std::set<CycleClosure, ClosureComparator> closures;

public:
    CycleClosureSet() = default;
    CycleClosureSet(const CycleClosureSet&) = delete;
    CycleClosureSet(CycleClosureSet&&) = default;

    bool add(const c_size idxA, const c_size idxB)
    {
        return closures.emplace(idxA, idxB, closures.size() + 1).second;
    }

    c_size size() const
    {
        return static_cast<c_size>(closures.size());
    }

    using Iterator = decltype(closures)::const_iterator;
    Iterator begin() const
    {
        return closures.begin();
    }
    Iterator end() const
    {
        return closures.end();
    }
};

std::string getCycleTagString(const c_size tag)
{
    return tag < 10 ?
        std::to_string(tag) :
        '%' + std::to_string(tag);
}

void rToSMILES(
    const BondedAtom* current, const BondedAtom* prev,
    std::vector<size_t>& insertPositions, CycleClosureSet& cycleClosures,
    std::string& smiles
)
{
    smiles += current->atom->getSMILES();
    insertPositions[current->index] = smiles.size();

    const auto neighbourCount = current->bonds.size();
    if (neighbourCount == 1)
        return;

    if (neighbourCount == 2)
    {
        const auto& nextBond = &current->bonds.front().getOther() == prev ?
            current->bonds.back() :
            current->bonds.front();
        const auto next = &nextBond.getOther();

        if (insertPositions[next->index] != std::string::npos)
        {
            if(cycleClosures.add(insertPositions[next->index], insertPositions[current->index]))
                smiles += nextBond.getSMILES() + getCycleTagString(cycleClosures.size());
            return;
        }

        smiles += nextBond.getSMILES();
        rToSMILES(next, current, insertPositions, cycleClosures, smiles);
        return;
    }

    for (c_size i = 0; i < neighbourCount - 2; ++i)
    {
        const auto& nextBond = current->bonds[i];
        const auto next = &nextBond.getOther();
        if (next == prev)
            continue;

        if (insertPositions[next->index] != std::string::npos)
        {
            if(cycleClosures.add(insertPositions[next->index], insertPositions[current->index]))
                smiles += nextBond.getSMILES() + getCycleTagString(cycleClosures.size());
            continue;
        }

        smiles += '(' + nextBond.getSMILES();
        rToSMILES(next, current, insertPositions, cycleClosures, smiles);
        smiles += ')';
    }

    const auto& secondToLastBond = current->bonds[neighbourCount - 2];
    const auto secondToLast = &secondToLastBond.getOther();
    const auto& lastBond = current->bonds.back();
    const auto last = &lastBond.getOther();

    if (last != prev)
    {
        if (secondToLast != prev)
        {
            if (insertPositions[secondToLast->index] != std::string::npos)
            {
                if(cycleClosures.add(insertPositions[secondToLast->index], insertPositions[current->index]))
                    smiles += secondToLastBond.getSMILES() + getCycleTagString(cycleClosures.size());
            }
            else
            {
                smiles += '(' + secondToLastBond.getSMILES();
                rToSMILES(secondToLast, current, insertPositions, cycleClosures, smiles);
                smiles += ')';
            }
        }

        if (insertPositions[last->index] != std::string::npos)
        {
            if(cycleClosures.add(insertPositions[last->index], insertPositions[current->index]))
                smiles += lastBond.getSMILES() + getCycleTagString(cycleClosures.size());
            return;
        }

        smiles += lastBond.getSMILES();
        rToSMILES(last, current, insertPositions, cycleClosures, smiles);
        return;
    }

    if (insertPositions[secondToLast->index] != std::string::npos)
    {
        if (cycleClosures.add(insertPositions[secondToLast->index], insertPositions[current->index]))
            smiles += secondToLastBond.getSMILES() + getCycleTagString(cycleClosures.size());
        return;
    }

    smiles += secondToLastBond.getSMILES();
    rToSMILES(secondToLast, current, insertPositions, cycleClosures, smiles);
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
    smiles += current->atom->getSMILES();

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
        {
            const auto& nextBond = current->bonds[i];
            const auto next = &nextBond.getOther();


            if (insertPositions[next->index] != std::string::npos)
            {
                if (cycleClosures.add(insertPositions[next->index], insertPositions[current->index]))
                    smiles += nextBond.getSMILES() + getCycleTagString(cycleClosures.size());
                continue;
            }

            smiles += '(' + nextBond.getSMILES();
            rToSMILES(next, current, insertPositions, cycleClosures, smiles);
            smiles += ')';
        }

        const auto& lastBond = current->bonds.back();
        const auto last = &lastBond.getOther();

        if (insertPositions[last->index] == std::string::npos)
        {
            // Since this is the first atom we dont' have to check 'if(cycleClosures.add(next->index, current->index))'.
            // We alredy know this isn't a new cycle.
            smiles += current->bonds.back().getSMILES();
            rToSMILES(&current->bonds.back().getOther(), current, insertPositions, cycleClosures, smiles);
        }
    }

    for (const auto& closure : cycleClosures)
        smiles.insert(closure.idxA, getCycleTagString(closure.tag));

    if(smiles.back() != ')')
        return smiles;

    // Strip unnecessary parenthesis resulted from cycles, aka trailing ')' and their paired '('.
    // TODO: This is a bit ineficient, smiles could be generated without these parenthesis.
    size_t idx = smiles.size() - 1;
    while (idx-- > 0 && smiles[idx] == ')');
    const size_t lastIdx = idx; // Last non-')' symbol.

    std::vector<size_t> removeIdx;
    std::stack<size_t> parethesisStack;
    while (idx-- > 0)
    {
        if (smiles[idx] == ')')
            parethesisStack.push(idx);
        else if (smiles[idx] == '(')
        {
            if (parethesisStack.empty())
                removeIdx.emplace_back(idx); // These are naturally sorted in reversed order.
            else
                parethesisStack.pop();
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

std::optional<MolecularStructure> MolecularStructure::create(const std::string& smiles)
{
    MolecularStructure temp(smiles);
    return temp.isEmpty() ? std::nullopt : std::optional(std::move(temp));
}
