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
                atoms[prev]->bonds.emplace_back(Bond(*atoms.back(), bondType));
                atoms.back()->bonds.emplace_back(Bond(*atoms[prev], bondType));
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
                atoms[prev]->bonds.emplace_back(Bond(*atoms.back(), bondType));
                atoms.back()->bonds.emplace_back(Bond(*atoms[prev], bondType));
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

            // TODO: uint8_t might not be enough
            const uint8_t label = smiles[i + 1] * 10 + smiles[i + 2] - 11 * '0';
            if (rings.contains(label))
            {
                atoms[rings[label]]->bonds.emplace_back(Bond(*atoms.back(), bondType));
                atoms.back()->bonds.emplace_back(Bond(*atoms[rings[label]], bondType));
                bondType = BondType::SINGLE;
            }
            else
                rings.emplace(label, atoms.size() - 1);
            
            i += 2;
            continue;
        }

        if (isdigit(smiles[i]))
        {
            uint8_t label = smiles[i] - '0';
            if (rings.contains(label))
            {
                atoms[rings[label]]->bonds.emplace_back(Bond(*atoms.back(), bondType));
                atoms.back()->bonds.emplace_back(Bond(*atoms[rings[label]], bondType));
            }
            else
                rings.emplace(label, atoms.size() - 1);

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
                    //lhsPrecedence == rhsPrecedence ? lhs.getValence() > rhs.getValence() : TODO: this causes issues with smiles print
                    false;
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
            if (sdMapping.contains(bond.getOther().index))
            {
                // TODO: should have an addBond() for this
                destination.atoms.back()->bonds.emplace_back(Bond(
                    *destination.atoms[sdMapping.at(bond.getOther().index)],
                    bond.getType())
                );

                destination.atoms[sdMapping.at(bond.getOther().index)]->bonds.emplace_back(Bond(
                    *destination.atoms.back(),
                    bond.getType())
                );
            }
            else if(not sourceIgnore.contains(bond.getOther().index))
            {
                queue.push(bond.getOther().index);
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

std::string MolecularStructure::rToSMILES(
    const BondedAtom* current, const BondedAtom* prev,
    std::vector<size_t>& insertPositions,
    uint8_t& cycleCount,
    std::map<c_size, uint8_t>& cycleHeads,
    const size_t insertOffset) const
{
    std::string smiles;
    while (true)
    {
        smiles += current->atom->getSMILES();
        insertPositions[current->index] = insertOffset + smiles.size();

        if (current->bonds.size() <= 1)  // C-C-C     end of chain, only neighbour == prev
            break;                       //     ^

        if (current->bonds.size() == 2)   // C-C-C
        {                                 //   ^
            const auto& bondToNext = not current->bonds.front().getOther().isSame(*prev) ?
                current->bonds.front() :
                current->bonds.back();

            if (insertPositions[bondToNext.getOther().index] != std::string::npos)  // cycle end
            {
                const auto [_, newHead] = cycleHeads.emplace(bondToNext.getOther().index, cycleCount);
                smiles += std::to_string(cycleCount);

                if (newHead)
                    ++cycleCount;

                break;
            }
            else
            {
                smiles += bondToNext.getSMILES();
                prev = current;
                current = &bondToNext.getOther();
            }

            continue;
        }
                                                                //   C
        for (c_size i = 0; i < current->bonds.size() - 1; ++i)  //   |
        {                                                       // C-C-C
            if (current->bonds[i].getOther().isSame(*prev))     //   ^
                continue;

            if (insertPositions[current->bonds[i].getOther().index] != std::string::npos)     // cycle end
            {
                if (smiles.back() == ')')
                    continue;

                const auto [_, newHead] = cycleHeads.emplace(current->bonds[i].getOther().index, cycleCount);
                smiles += std::to_string(cycleCount);

                if (newHead)
                    ++cycleCount;
            }
            else
            {
                if (current->bonds.size() >= 4)
                {
                    const auto temp = rToSMILES(&current->bonds[i].getOther(), current, insertPositions, cycleCount, cycleHeads, insertOffset + smiles.size() + 1);
                    smiles += '(' + current->bonds[i].getSMILES() + temp + ')';
                }
                else
                {
                    const auto before = cycleCount;
                    const auto temp = rToSMILES(&current->bonds[i].getOther(), current, insertPositions, cycleCount, cycleHeads, insertOffset + smiles.size());
                    if (cycleCount == before || current->bonds.size() >= 4)
                        smiles += '(' + current->bonds[i].getSMILES() + temp + ')';
                    else
                        smiles += current->bonds[i].getSMILES() + temp;
                }
            }
        }

        if (insertPositions[current->bonds.back().getOther().index] != std::string::npos)  // cycle end
        {
            break;
        }
        else
        {
            smiles += current->bonds.back().getSMILES();
            prev = current;
            current = &current->bonds.back().getOther();
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

    smiles += atoms.front()->atom->getSMILES();
    insertPositions.front() = smiles.size();

    if (atoms.front()->bonds.empty())
        return smiles;

    if (atoms.front()->bonds.size() == 1)
    {
        smiles += atoms.front()->bonds.front().getSMILES();
        smiles += rToSMILES(&atoms.front()->bonds.front().getOther(), atoms.front().get(), insertPositions, cycleCount, cycleHeads, smiles.size());
        insertCycleHeads(smiles, insertPositions, cycleHeads);
        return smiles;
    }

    for (c_size i = 0; i < atoms.front()->bonds.size() - 1; ++i)
    {        
        const auto before = cycleCount;
        const auto temp = rToSMILES(&atoms.front()->bonds[i].getOther(), atoms.front().get(), insertPositions, cycleCount, cycleHeads, smiles.size());
        if(cycleCount == before)
            smiles += '(' + atoms.front()->bonds[i].getSMILES() + temp + ')';
        else
            smiles += atoms.front()->bonds[i].getSMILES() + temp;
    }

    if (insertPositions[atoms.front()->bonds.back().getOther().index] != std::string::npos)  // cycle end
    {
        insertCycleHeads(smiles, insertPositions, cycleHeads);
        return smiles;
    }
    else
    {
        smiles += atoms.front()->bonds.back().getSMILES();
        const auto temp = rToSMILES(&atoms.front()->bonds.back().getOther(), atoms.front().get(), insertPositions, cycleCount, cycleHeads, smiles.size());
        smiles += atoms.front()->bonds.back().getSMILES() + temp;
    }

    insertCycleHeads(smiles, insertPositions, cycleHeads);
    return smiles;
}

std::optional<MolecularStructure> MolecularStructure::create(const std::string& smiles)
{
    MolecularStructure temp(smiles);
    return temp.isEmpty() ? std::nullopt : std::optional(std::move(temp));
}
