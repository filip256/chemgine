#include <array>
#include <algorithm>
#include <queue>

#include "MolecularStructure.hpp"
#include "CompositeComponent.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"

MolecularStructure::MolecularStructure(const std::string& smiles)
{
    if (loadFromSMILES(smiles) == false)
    {
        Logger::log("Molecule initialized with ill-formed smiles: \"" + smiles + "\".", LogType::BAD);
        return;
    }
    normalize();
}

MolecularStructure::MolecularStructure(const std::string& serialized, const bool renormalize)
{
    deserialize(serialized);

    if(renormalize)
        normalize();
}

MolecularStructure::MolecularStructure(const MolecularStructure& other) noexcept :
    impliedHydrogenCount(other.impliedHydrogenCount),
    bonds(other.bonds.size())
{
    this->components.reserve(other.components.size());
    for (size_t i = 0; i < other.components.size(); ++i)
        this->components.emplace_back(other.components[i]->clone());

    for (size_t i = 0; i < other.bonds.size(); ++i)
    {
        this->bonds[i].reserve(other.bonds[i].size());
        for (size_t j = 0; j < other.bonds[i].size(); ++j)
            this->bonds[i].emplace_back(new Bond(*other.bonds[i][j]));
    }
}

MolecularStructure::MolecularStructure(const BaseComponent& component) noexcept:
    impliedHydrogenCount(component.data().valence)
{
    components.emplace_back(component.clone());
    bonds.emplace_back();
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
            if (Atom::isDefined(smiles[i]))
            {
                components.emplace_back(new Atom(smiles[i]));
            }
            else if (Atom::isDefined(smiles.substr(i, 2)))
            {
                components.emplace_back(new Atom(smiles.substr(i, 2)));
                ++i;
            }
            else
            {
                Logger::log("Atomic symbol '" + std::string(1, smiles[i]) + "' at " + std::to_string(i) + " is undefined.", LogType::BAD);
                clear();
                return false;
            }

            bonds.emplace_back(std::vector<Bond*>());

            if (prev != npos)
            {
                bonds[prev].emplace_back(new Bond(components.size() - 1, bondType));
                bonds[bonds.size() - 1].emplace_back(new Bond(prev, bondType));
                bondType = BondType::SINGLE;
            }

            prev = components.size() - 1;
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
                Logger::log("Unpaired ')' found at " + std::to_string(i) + " .", LogType::BAD);
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
                Logger::log("Unpaired '[' found at " + std::to_string(i) + " .", LogType::BAD);
                clear();
                return false;
            }

            if (Atom::isDefined(smiles.substr(i + 1, t - i - 1)) == false)
            {
                Logger::log("Atomic symbol '" + smiles.substr(i + 1, t - i - 1) + "' at " + std::to_string(i) + "is undefined.", LogType::BAD);
                clear();
                return false;
            }
            
            components.emplace_back(new Atom(smiles.substr(i + 1, t - i - 1)));
            bonds.emplace_back(std::vector<Bond*>());

            if (prev != npos)
            {
                bonds[prev].emplace_back(new Bond(components.size() - 1, bondType));
                bonds[bonds.size() - 1].emplace_back(new Bond(prev, bondType));
                bondType = BondType::SINGLE;
            }

            prev = components.size() - 1;
            i = t;
            continue;
        }

        if (smiles[i] == '%')
        {
            if (i + 2 >= smiles.size() || isdigit(smiles[i + 1]) == false || isdigit(smiles[i + 2]) == false)
            {
                Logger::log("Two-digit ring label is missing.", LogType::BAD);
                clear();
                return false;
            }

            uint8_t label = smiles[i + 1] * 10 + smiles[i + 2] - 11 * '0';
            if (rings.contains(label))
            {
                bonds[rings[label]].emplace_back(new Bond(components.size() - 1, bondType));
                bonds[components.size() - 1].emplace_back(new Bond(rings[label], bondType));
                bondType = BondType::SINGLE;
            }
            else
            {
                rings.emplace(std::move(std::make_pair(label, components.size() - 1)));
            }
            
            i += 2;
            continue;
        }

        if (isdigit(smiles[i]))
        {
            uint8_t label = smiles[i] - '0';
            if (rings.contains(label))
            {
                bonds[rings[label]].emplace_back(new Bond(components.size() - 1, bondType));
                bonds[components.size() - 1].emplace_back(new Bond(rings[label], bondType));
            }
            else
            {
                rings.emplace(std::move(std::make_pair(label, components.size() - 1)));
            }
            continue;
        }

        Logger::log("Unidentified symbol '" + std::string(1, smiles[i]) + "' at " + std::to_string(i) + " found.", LogType::BAD);
        clear();
        return false;
    }

    if(branches.empty() == false)
    {
        Logger::log("Unpaired '(' found.", LogType::BAD);
        clear();
        return false;
    }

    const auto hCount = countImpliedHydrogens();
    if(hCount == -1)
    {
        Logger::log("Valence of a component was exceeded.", LogType::BAD);
        clear();
        return false;
    }
    impliedHydrogenCount = hCount;

    return true;
}

void MolecularStructure::normalize()
{
    if (components.size() <= 1)
        return;

    std::vector<const BaseComponent*> copy(components.data());
    std::vector<c_size> map(components.size(), npos);


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
            if (copy[j] == components[i])
                map[i] = j;
    }

    components = std::move(copy);

    for (c_size i = 0; i < bonds.size(); ++i)
        for (c_size j = 0; j < bonds[i].size(); ++j)
            bonds[i][j]->other = map[bonds[i][j]->other];

    for (c_size i = 0; i < components.size(); ++i)
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
                components[bonds[b][j]->other]->getPrecedence() < components[k->other]->getPrecedence())
            {
                bonds[b][j + 1] = bonds[b][j];
                --j;
            }
            bonds[b][j + 1] = k;
        }
    }
}

int16_t MolecularStructure::countImpliedHydrogens() const
{
    int16_t hCount = 0;
    for (c_size i = 0; i < components.size(); ++i)
    {
        if (components[i]->isRadicalType())
            continue;

        auto v = components[i]->data().valence;
        if (bonds[i].size() > v)
            return -1;

        auto cnt = getDegreeOf(i);
        if (cnt > v)
            return -1;

        hCount += v - cnt;
    }
    return hCount;
}

uint8_t MolecularStructure::getDegreeOf(const c_size idx) const
{
    uint8_t cnt = 0;
    for (c_size i = 0; i < bonds[idx].size(); ++i)
        cnt += bonds[idx][i]->getValence();
    return cnt;
}

const BaseComponent* MolecularStructure::getComponent(const c_size idx) const
{
    return components[idx];
}

c_size MolecularStructure::getImpliedHydrogenCount() const
{
    return impliedHydrogenCount;
}

double MolecularStructure::getMolarMass() const
{
    double cnt = 0;
    for (c_size i = 0; i < components.size(); ++i)
    {
        cnt += components[i]->data().weight;
    }
    cnt += impliedHydrogenCount * Atom('H').data().weight;
    return cnt;
}

uint8_t MolecularStructure::getDegreesOfFreedom() const
{
    if (components.size() == 1) // monoatomics have 3
        return 3;

    if (components.size() == 2 || isVirtualHydrogen()) // diatomics have 5
        return 5;

    // TODO: find degreesOfFreedom for other molecules (6 is true for CH4)
    return 6;
}

c_size MolecularStructure::getRadicalAtomsCount() const
{
    c_size cnt = 0;
    c_size i = components.size();
    while (i-- > 0 && components[i]->isRadicalType())
        ++cnt;

    return cnt;
}

bool MolecularStructure::isComplete() const
{
    return components.empty () || !components.back()->isRadicalType();
}

std::unordered_map<ComponentIdType, c_size> MolecularStructure::getComponentCountMap() const
{
    std::unordered_map<ComponentIdType, c_size> result;
    for (c_size i = 0; i < components.size(); ++i)
    {
        if (result.contains(components[i]->getId()))
            ++result[components[i]->getId()];
        else
            result.emplace(std::move(std::make_pair(components[i]->getId(), 1)));
    }
    
    if (impliedHydrogenCount == 0)
        return result;

    const auto hId = Atom("H").getId();
    if (result.contains(hId))
        result[hId] += impliedHydrogenCount;
    else
        result.emplace(std::move(std::make_pair(hId, impliedHydrogenCount)));

    return result;
}

bool MolecularStructure::isEmpty() const
{
    return impliedHydrogenCount == 0 && components.empty();
}

c_size MolecularStructure::componentCount() const
{
    return components.size();
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
    return virtualBondCount() / 2 > components.size() - 1;
}

bool MolecularStructure::isConnected() const
{
    if (components.size() == 0)
        return true;

    if (virtualBondCount() / 2 < components.size() - 1)
        return false;

    std::vector<uint8_t> visited;
    visited.resize(components.size(), false);
    std::queue<c_size> queue;

    c_size c = 0;

    while (true)
    {
        visited[c] = true;

        for(c_size i = 0; i < bonds[c].size(); ++i)
            if (visited[bonds[c][i]->other] == false) 
                queue.push(bonds[c][i]->other);

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
    return components.size() == 0 && impliedHydrogenCount == 2;
}

bool MolecularStructure::areAdjacent(const c_size idxA, const c_size idxB) const
{
    for (c_size i = 0; i < bonds[idxA].size(); ++i)
        if (idxB == bonds[idxA][i]->other)
            return true;
    return false;
}

void MolecularStructure::rPrint(
    std::vector<std::string>& buffer,
    const size_t x,
    const size_t y,
    const c_size c,
    std::vector<uint8_t>& visited) const
{
    if (x < 1 || y < 1 ||
        x >= buffer[0].size() - 2 || y >= buffer.size() - 2 ||
        buffer[y][x] != ' ')
        return;

    visited[c] = true;

    for(c_size i = 0; i < components[c]->data().symbol.size() && x + i < buffer[0].size(); ++i)
        buffer[y][x + i] = components[c]->data().symbol[i];

    for (c_size i = 0; i < bonds[c].size(); ++i)
        if (visited[bonds[c][i]->other] == false)
        {
            char vC = '³', hC = 'Ä';
            if (bonds[c][i]->type == BondType::DOUBLE)
                vC = 'º', hC = 'Í';
            else if (bonds[c][i]->type == BondType::TRIPLE)
                vC = 'ð', hC = 'ð';


            if (buffer[y][x + 2] == ' ')
            {
                buffer[y][x + 1] = hC;
                rPrint(buffer, x + 2, y, bonds[c][i]->other, visited);
            }
            else if (buffer[y][x - 2] == ' ')
            {
                buffer[y][x - 1] = hC;
                rPrint(buffer, x - 2, y, bonds[c][i]->other, visited);
            }
            else if (buffer[y - 2][x] == ' ')
            {
                buffer[y - 1][x] = vC;
                rPrint(buffer, x, y - 2, bonds[c][i]->other, visited);
            }
            else if (buffer[y + 2][x] == ' ')
            {
                buffer[y + 1][x] = vC;
                rPrint(buffer, x, y + 2, bonds[c][i]->other, visited);
            }
            else
                break;
        }
}

std::string MolecularStructure::print(const size_t maxWidth, const size_t maxHeight) const
{
    if (components.empty())
    {
        if (isVirtualHydrogen())
            return std::string(maxWidth / 4, ' ') + "HÄH\n";
        return "";
    }

	std::vector<std::string> buffer(maxHeight, std::string(maxWidth, ' '));
    std::vector<uint8_t> visited(components.size(), false);
    rPrint(buffer, buffer[0].size() / 4, buffer.size() / 4, 0, visited);

    std::string str;
    size_t i = 0;
    while (buffer[i].find_first_not_of(' ') == std::string::npos) ++i;

    do
    {
        str += buffer[i] + '\n';
        ++i;
    }
    while (buffer[i].find_first_not_of(' ') != std::string::npos);

    return str;
}


void MolecularStructure::clear()
{
    components.clear();
    while (bonds.size())
    {
        bonds.back().clear();
        bonds.pop_back();
    }
    impliedHydrogenCount = 0;
}

MolecularStructure MolecularStructure::createCopy() const
{
    return MolecularStructure(*this);
}

bool MolecularStructure::areMatching(
    const c_size idxA, const MolecularStructure& a,
    const c_size idxB, const MolecularStructure& b)
{
    if (a.bonds[idxA].size() != b.bonds[idxB].size())
        return false;

    if (BaseComponent::areEqual(*a.components[idxA], *(b.components[idxB])) == false)
        return false;

    return true;
}

bool MolecularStructure::areMatching(
    const Bond& nextA, const MolecularStructure& a,
    const Bond& nextB, const MolecularStructure& b,
    const bool escapeRadicalTypes)
{
    if (nextA.type != nextB.type)
        return false;

    // escape radical types
    if (escapeRadicalTypes == true && b.components[nextB.other]->isRadicalType())
    {
        return true;
    }

    if (areMatching(nextA.other, a, nextB.other, b) == false)
        return false;

    // test to see both have the same types of bonds
    std::array<int8_t, 5> counts{ 0 };
    for (c_size i = 0; i < a.bonds[nextA.other].size(); ++i)
    {
        ++counts[a.bonds[nextA.other][i]->getValence()];
        --counts[b.bonds[nextB.other][i]->getValence()];
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
        ++counts[a.bonds[idxA][i]->getValence()];
    for (c_size i = 0; i < b.bonds[idxB].size(); ++i)
        --counts[b.bonds[idxB][i]->getValence()];

    for (uint8_t i = 0; i < counts.size(); ++i)
        score -= counts[i] * scorePerBond;

    return score;
}

uint8_t MolecularStructure::maximalSimilarity(
    const Bond& nextA, const MolecularStructure& a,
    const Bond& nextB, const MolecularStructure& b)
{
    if (nextA.type != nextB.type)
        return 0;

    if (BaseComponent::areEqual(*a.components[nextA.other], *(b.components[nextB.other])) == false)
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
        const Bond& next = *b.bonds[idxB][i];
        if (visitedB[next.other])
        {
            // here we could mark the nodes contained in cycles
            continue;
        }

        bool matchFound = false;
        for (c_size j = 0; j < a.bonds[idxA].size(); ++j)
        {
            if (mapping.contains(a.bonds[idxA][j]->other) ||  // do not reuse already mapped nodes
                areMatching(*a.bonds[idxA][j], a, next, b, escapeRadicalTypes) == false)
                continue;

            if (DFSCompare(a.bonds[idxA][j]->other, a, next.other, b, visitedB, mapping, escapeRadicalTypes))
            {
                matchFound = true;
                break;
            }
            
            // revert wrong branch
            visitedB[next.other] = false;
            mapping.erase(a.bonds[idxA][j]->other);
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
        const Bond& next = *b.bonds[idxB][i];
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
            if (mappedA.contains(a.bonds[idxA][j]->other)) // do not reuse already mapped nodes
                continue;

            const auto score = maximalSimilarity(*a.bonds[idxA][j], a, next, b);
            if (score == 0)
                continue;

            // reversing bad branches isn't possible here, so copies are needed :(
            auto mappedACopy = mappedA;
            auto mappedBCopy = mappedB;
            auto subMap = DFSMaximal(a.bonds[idxA][j]->other, a, mappedACopy, next.other, b, mappedBCopy);
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
    if (pattern.componentCount() == 0 || this->componentCount() == 0)
        return std::unordered_map<c_size, c_size>();

    // should start with a non radical type
    c_size pStart = 0;
    while (pStart < pattern.componentCount() && pattern.components[pStart]->isRadicalType())
        ++pStart;
    if (pStart == pattern.componentCount())
        pStart = 0;


    for (c_size i = 0; i < this->componentCount(); ++i)
    {
        if (areMatching(i, *this, pStart, pattern))
        {
            std::vector<uint8_t> visited(pattern.componentCount(), false);
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
    if (pattern.componentCount() == 0 || this->componentCount() == 0)
        return std::pair<std::unordered_map<c_size, c_size>, uint8_t>();

    // find atom that matches in both target and pattern
    std::pair<std::unordered_map<c_size, c_size>, uint8_t> maxMapping;
    uint8_t maxScore = 0;
    for (c_size i = 0; i < this->components.size(); ++i)
    {
        if (targetIgnore.contains(i))
            continue;

        // skip some comparisons
        if (maxMapping.first.contains(i))
            continue;

        for (c_size j = 0; j < pattern.components.size(); ++j)
        {
            if (patternIgnore.contains(j))
                continue;

            if (BaseComponent::areEqual(*this->components[i], *(pattern.components[j])) == false)
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
    bool renormalize,
    const std::unordered_set<c_size>& sourceIgnore)
{
    // overwrites first matching radical atom
    if (destination.components[sdMapping[sourceIdx]]->isRadicalType())
        destination.components.replace(sdMapping[sourceIdx], source.components[sourceIdx]->clone());

    std::queue<c_size> queue;
    for (c_size i = 0; i < source.bonds[sourceIdx].size(); ++i)
        if(sdMapping.contains(source.bonds[sourceIdx][i]->other) == false && sourceIgnore.contains(source.bonds[sourceIdx][i]->other) == false)
            queue.push(source.bonds[sourceIdx][i]->other);

    if (queue.empty())
        return;

    while (queue.size()) 
    {
        const auto c = queue.front();
        queue.pop();

        // add current node
        sdMapping.insert(std::make_pair(c, destination.components.size()));
        destination.components.emplace_back(source.components[c]->clone());
        destination.bonds.emplace_back(std::vector<Bond*>());

        // add bonds to existing nodes and queue non-existing nodes
        for (c_size i = 0; i < source.bonds[c].size(); ++i)
        {
            if (sdMapping.contains(source.bonds[c][i]->other))
            {
                destination.bonds.back().emplace_back(new Bond(
                    sdMapping.at(source.bonds[c][i]->other),
                    source.bonds[c][i]->type)
                );

                destination.bonds[sdMapping.at(source.bonds[c][i]->other)].emplace_back(new Bond(
                    destination.components.size() - 1,
                    source.bonds[c][i]->type)
                );
            }
            else if(sourceIgnore.contains(source.bonds[c][i]->other) == false)
            {
                queue.push(source.bonds[c][i]->other);
            }
        }
    }

    if (renormalize)
    {
        destination.normalize();
        destination.recountImpliedHydrogens();
        sdMapping.clear();
    }
}

MolecularStructure MolecularStructure::addSubstituents(
    const MolecularStructure& pattern,
    const MolecularStructure& instance,
    std::unordered_map<c_size, c_size>& ipMap,
    bool renormalize)
{
    MolecularStructure result(pattern);

    for (auto const& p : ipMap)
    {
        copyBranch(result, instance, p.first, ipMap, false);
    }

    if (renormalize)
    {
        result.normalize();
        result.recountImpliedHydrogens();
        ipMap.clear();
    }

    return result;
}

bool MolecularStructure::operator==(const MolecularStructure& other) const
{
    if (this->componentCount() != other.componentCount() || this->impliedHydrogenCount != other.impliedHydrogenCount)
        return false;

    const auto mapping = this->mapTo(other, false);
    return mapping.size() == this->componentCount();
}

bool MolecularStructure::operator!=(const MolecularStructure& other) const
{
    if (this->componentCount() != other.componentCount() || this->impliedHydrogenCount != other.impliedHydrogenCount)
        return true;

    const auto mapping = this->mapTo(other, false);
    return mapping.size() != this->componentCount();
}

bool MolecularStructure::operator==(const std::string& other) const
{
    return *this == MolecularStructure(other);
}

bool MolecularStructure::operator!=(const std::string& other) const
{
    return *this != MolecularStructure(other);
}



std::string MolecularStructure::rToSMILES(c_size c, c_size prev, std::vector<uint8_t>& visited, uint8_t& cycleCount) const
{
    std::string r;
    while (true)
    {
        visited[c] = true;
        r += components[c]->data().symbol;

        if (bonds[c].empty())
            break;

        if (bonds[c].size() == 1)
        {
            if (visited[bonds[c][0]->other] == false)
            {
                r += Bond::toSMILES(bonds[c][0]->type);
                c = bonds[c][0]->other;
            }
            else
                break;
        }
        else
        {
            for (c_size i = 0; i < bonds[c].size() - 1; ++i)
            {
                if (bonds[c][i]->other == prev)
                    continue;

                if (visited[bonds[c][i]->other] == false)
                {
                    const auto cc = cycleCount;
                    const auto temp = rToSMILES(bonds[c][i]->other, c, visited, cycleCount);
                    if (cc == cycleCount)
                        r += '(' + Bond::toSMILES(bonds[c][i]->type) + temp + ')';
                    else
                        r += Bond::toSMILES(bonds[c][i]->type) + temp;
                }
                else
                {
                    ++cycleCount;
                    r = std::to_string(cycleCount) + r;
                    r += std::to_string(cycleCount);
                }
            }

            if (visited[bonds[c].back()->other] == false)
            {
                r += Bond::toSMILES(bonds[c].back()->type);
                c = bonds[c].back()->other;
            }
            else
                break;
        }
    }
    return r;

    /*std::string r;
    while (true)
    {
        if (visited[c])
        {
            r = "1" + r;
            r += "1";
            break;
        }
        visited[c] = true;

        r += components[c]->data().symbol;
        if (bonds[c].size() == 1)
        {
            if (visited[bonds[c][0]->other] == false)
            {
                r += Bond::toSMILES(bonds[c][0]->type);
                c = bonds[c][0]->other;
            }
            else
                break;
        }
        else if (bonds[c].empty())
        {
            break;
        }
        else
        {
            for (c_size i = 0; i < bonds[c].size() - 1; ++i)
            {
                if (visited[bonds[c][i]->other] == false && bonds[c][i]->other != c)
                {
                    r += '(' + Bond::toSMILES(bonds[c][i]->type) + rToSMILES(bonds[c][i]->other, visited) + ')';
                }
            }
            r += Bond::toSMILES(bonds[c].back()->type) + rToSMILES(bonds[c].back()->other, visited);
        }
    }
    return r;*/
}

std::string MolecularStructure::toSMILES() const
{
    if (isVirtualHydrogen()) // pure virtual hydrogen
        return "HH";

    std::vector<uint8_t> visited(components.size(), false);
    uint8_t cycleCount = 0;
    return rToSMILES(0, 0, visited, cycleCount);
}


std::string MolecularStructure::serialize() const
{
    if (isVirtualHydrogen()) // pure virtual hydrogen
        return "h";

    std::string result;
    for (c_size i = 0; i < components.size(); ++i)
        result += std::to_string(components[i]->getId()) + ';';
    result.back() = '_';
    for (c_size i = 0; i < components.size(); ++i)
    {
        for (c_size j = 0; j < bonds[i].size(); ++j)
            result += bonds[i][j]->toSMILES() + std::to_string(bonds[i][j]->other) + ';';
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

    const auto tokens = DataHelpers::parseList(str, '_');
    if (tokens.empty())
    {
        clear();
        return false;
    }

    auto comps = DataHelpers::parseList(tokens[0], ';');
    for (c_size i = 0; i < comps.size(); ++i)
    {
        const auto id = DataHelpers::toUInt(comps[i]);
        if (id.has_value() == false || Atom::isDefined(static_cast<ComponentIdType>(id.value())) == false)
        {
            clear();
            return false;
        }

        components.emplace_back(new Atom(static_cast<ComponentIdType>(id.value())));
    }

    for (c_size i = 1; i < tokens.size(); ++i)
    {
        comps = DataHelpers::parseList(tokens[i], ';');
        bonds.emplace_back(std::vector<Bond*>());
        for (c_size j = 0; j < comps.size(); ++j)
        {
            if (comps[j].empty())
            {
                clear();
                return false;
            }

            if (std::isdigit(comps[j][0]))
            {
                const auto id = DataHelpers::toUInt(comps[j]);
                if (id.has_value() == false)
                {
                    clear();
                    return false;
                }

                bonds[i - 1].emplace_back(new Bond(id.value(), BondType::SINGLE));
            }
            else
            {
                const auto bType = Bond::fromSMILES(comps[j][0]);
                if (bType == BondType::NONE)
                {
                    clear();
                    return false;
                }

                const auto id = DataHelpers::toUInt(comps[j].substr(1));
                if (id.has_value() == false)
                {
                    clear();
                    return false;
                }

                  bonds[i - 1].emplace_back(new Bond(id.value(), bType));
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