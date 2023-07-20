#include <array>

#include "MolecularStructure.hpp"
#include "CompositeComponent.hpp"
#include "Logger.hpp"
#include <algorithm>

MolecularStructure::MolecularStructure(const std::string& smiles)
{
    loadFromSMILES(smiles);
    //normalize();
}

MolecularStructure::~MolecularStructure()
{
    clear();
}

bool MolecularStructure::loadFromSMILES(const std::string& smiles)
{
    clear();

    std::unordered_map<uint8_t, size_t> rings;
    std::stack<size_t> branches;

    size_t prev = std::string::npos;
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

            bonds.emplace_back(std::move(std::vector<Bond*>()));

            if (prev != std::string::npos)
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
            bonds.emplace_back(std::move(std::vector<Bond*>()));

            if (prev != std::string::npos)
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

    const auto hCount = getHCount();
    if(hydrogenCount == -1)
    {
        Logger::log("Valence of a component was exceeded.", LogType::BAD);
        clear();
        return false;
    }
    hydrogenCount = hCount;

    return true;
}


void MolecularStructure::normalize()
{
    if (components.size() <= 1)
        return;

    std::vector<const BaseComponent*> copy(components);
    std::vector<size_t> map(components.size(), npos);


    for (size_t i = 1; i < copy.size(); ++i)
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
    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < copy.size(); ++j)
            if (copy[j] == components[i])
                map[i] = j;
    }

    components = copy;

    for (size_t i = 0; i < bonds.size(); ++i)
        for (size_t j = 0; j < bonds[i].size(); ++j)
            bonds[i][j]->other = map[bonds[i][j]->other];

    for (size_t i = 0; i < components.size(); ++i)
    {
        if (map[i] == npos || map[i] == i)
            continue;

        size_t p = i;
        auto pB = std::move(bonds[p]);
        do
        {
            auto tempB = std::move(bonds[map[p]]);
            bonds[map[p]] = std::move(pB);
            pB = std::move(tempB);

            const size_t tempP = p;
            p = map[p];
            map[tempP] = npos;
        } while (map[p] != npos);
    }

    for (size_t b = 0; b < bonds.size(); ++b)
    {
        for (size_t i = 1; i < bonds[b].size(); ++i)
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

int16_t MolecularStructure::getHCount() const
{
    int16_t hCount = 0;
    for (size_t i = 0; i < components.size(); ++i)
    {
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

uint8_t MolecularStructure::getDegreeOf(const size_t idx) const
{
    uint8_t cnt = 0;
    for (size_t i = 0; i < bonds[idx].size(); ++i)
        cnt += bonds[idx][i]->getValence();
    return cnt;
}

const BaseComponent* MolecularStructure::getComponent(const size_t idx) const
{
    return components[idx];
}

uint16_t MolecularStructure::getHydrogenCount() const
{
    return hydrogenCount;
}

double MolecularStructure::getMolarMass() const
{
    double cnt = 0;
    for (size_t i = 0; i < components.size(); ++i)
    {
        cnt += components[i]->data().weight;
    }
    cnt += hydrogenCount * Atom('H').data().weight;
    return cnt;
}

uint16_t MolecularStructure::getRadicalAtomsCount() const
{
    uint16_t cnt = 0;
    for (size_t i = 0; i < components.size(); ++i)
        if (components[i]->isAtomicType() &&
            static_cast<const AtomicComponent*>(components[i])->isRadicalType())
            ++cnt;
    return cnt;
}

bool MolecularStructure::isComplete() const
{
    // it does not search sub components
    for (size_t i = 0; i < components.size(); ++i)
        if (components[i]->isAtomicType() &&
            static_cast<const AtomicComponent*>(components[i])->isRadicalType())
                return false;
    return true;
}

size_t MolecularStructure::componentCount() const
{
    return components.size();
}

size_t MolecularStructure::bondCount() const
{
    size_t cnt = 0;
    for (size_t i = 0; i < bonds.size(); ++i)
        cnt += bonds[i].size();
    return cnt;
}

bool MolecularStructure::isCyclic() const
{
    return bondCount() > componentCount() - 1;
}

bool MolecularStructure::areAdjacent(const size_t idxA, const size_t idxB) const
{
    for (size_t i = 0; i < bonds[idxA].size(); ++i)
        if (idxB == bonds[idxA][i]->other)
            return true;
    return false;
}

void MolecularStructure::rPrint(
    std::vector<std::string>& buffer,
    const size_t x,
    const size_t y,
    const size_t c,
    std::vector<uint8_t>& visited) const
{
    if (x < 1 || y < 1 ||
        x >= buffer[0].size() - 2 || y >= buffer.size() - 2 ||
        buffer[y][x] != ' ')
        return;

    visited[c] = true;

    for(size_t i = 0; i < components[c]->data().symbol.size() && x + i < buffer[0].size(); ++i)
        buffer[y][x + i] = components[c]->data().symbol[i];

    for (size_t i = 0; i < bonds[c].size(); ++i)
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
            else if (buffer[y - 2][x] == ' ')
            {
                buffer[y - 1][x] = vC;
                rPrint(buffer, x, y - 2, bonds[c][i]->other, visited);
            }
            else if (buffer[y][x - 2] == ' ')
            {
                buffer[y][x - 1] = hC;
                rPrint(buffer, x - 2, y, bonds[c][i]->other, visited);
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
        return "";

	std::vector<std::string> buffer(maxHeight, std::string(maxWidth, ' '));
    size_t x = buffer[0].size() / 4, y = buffer.size() / 4;
    std::vector<uint8_t> visited(components.size(), false);
    rPrint(buffer, x, y, 0, visited);

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
    while (components.empty() == false)
    {
        while (bonds[components.size() - 1].empty() == false)
        {
            delete bonds[components.size() - 1].back();
            bonds[components.size() - 1].pop_back();
        }
        delete components.back();
        components.pop_back();
    }
    hydrogenCount = 0;
}

bool MolecularStructure::areMatching(
    const size_t idxA, const MolecularStructure& a,
    const size_t idxB, const MolecularStructure& b)
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
    for (size_t i = 0; i < a.bonds[nextA.other].size(); ++i)
    {
        ++counts[a.bonds[nextA.other][i]->getValence()];
        --counts[b.bonds[nextB.other][i]->getValence()];
    }
    for (size_t i = 0; i < counts.size(); ++i)
        if (counts[i] != 0)
            return false;

    return true;
}

uint8_t MolecularStructure::getBondSimilarity(
    const size_t idxA, const MolecularStructure& a,
    const size_t idxB, const MolecularStructure& b)
{
    uint8_t score = 255;
    uint8_t scorePerBond = a.bonds[idxA].size() / 255;
    std::array<int8_t, 5> counts{ 0 };

    for (size_t i = 0; i < a.bonds[idxA].size(); ++i)
        ++counts[a.bonds[idxA][i]->getValence()];
    for (size_t i = 0; i < b.bonds[idxB].size(); ++i)
        --counts[b.bonds[idxB][i]->getValence()];

    for (size_t i = 0; i < counts.size(); ++i)
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
    size_t idxA, const MolecularStructure& a,
    size_t idxB, const MolecularStructure& b,
    std::vector<uint8_t>& visitedB,
    std::unordered_map<size_t, size_t>& mapping,
    bool escapeRadicalTypes)
{
    mapping.emplace(std::move(std::make_pair(idxA, idxB)));
    visitedB[idxB] = true;

    for (size_t i = 0; i < b.bonds[idxB].size(); ++i)
    {
        const Bond& next = *b.bonds[idxB][i];
        if (visitedB[next.other])
        {
            // here we could mark the nodes contained in cycles
            continue;
        }

        bool matchFound = false;
        for (size_t j = 0; j < a.bonds[idxA].size(); ++j)
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

std::pair<std::unordered_map<size_t, size_t>, uint8_t> MolecularStructure::DFSMaximal(
    size_t idxA, const MolecularStructure& a,
    std::unordered_set<size_t>& mappedA,
    size_t idxB, const MolecularStructure& b,
    std::unordered_set<size_t>& mappedB)
{
    std::pair<std::unordered_map<size_t, size_t>, uint8_t> newMap;
    newMap.first.emplace(std::move(std::make_pair(idxA, idxB)));
    mappedA.emplace(idxA);
    mappedB.emplace(idxB);

    for (size_t i = 0; i < b.bonds[idxB].size(); ++i)
    {
        const Bond& next = *b.bonds[idxB][i];
        if (mappedB.contains(next.other))
        {
            continue;
        }

        // only the largest mapping is added into the final but states need to be copied
        std::pair<std::unordered_map<size_t, size_t>, uint8_t> maxMapping;
        std::unordered_set<size_t> maxMappedA;
        std::unordered_set<size_t> maxMappedB;
        for (size_t j = 0; j < a.bonds[idxA].size(); ++j)
        {
            if (mappedA.contains(a.bonds[idxA][j]->other)) // do not reuse already mapped nodes
                continue;

            const auto score = maximalSimilarity(*a.bonds[idxA][j], a, next, b);
            if (score == 0)
                continue;

            // reversing bad branches isn't possible here, so copies are needed :(
            auto mappedACopy = mappedA;
            auto mappedBCopy = mappedB;
            auto subMap = DFSMaximal(a.bonds[idxA][j]->other, a, mappedBCopy, next.other, b, mappedBCopy);
            if (subMap.first.size() > maxMapping.first.size() ||
               (subMap.first.size() == maxMapping.first.size() && score > maxMapping.second))
            {
                maxMapping = std::move(subMap);
                maxMappedA = std::move(mappedACopy);
                maxMappedB = std::move(mappedBCopy);
            }
        }

        newMap.first.merge(maxMapping.first);
        newMap.second = maxMapping.second;
        mappedA.merge(maxMappedA);
        mappedB.merge(maxMappedB);
    }
    return newMap;
}

bool MolecularStructure::checkConnectivity(
    const MolecularStructure& target,
    const MolecularStructure& pattern,
    const std::unordered_map<size_t, size_t>& mapping)
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

std::unordered_map<size_t, size_t> MolecularStructure::mapTo(const MolecularStructure& pattern, bool escapeRadicalTypes) const
{
    if (pattern.componentCount() == 0 || this->componentCount() == 0)
        return std::unordered_map<size_t, size_t>();

    // should start with a non radical type
    size_t pStart = 0;
    while (pStart < pattern.componentCount() && pattern.components[pStart]->isRadicalType())
        ++pStart;
    if (pStart == pattern.componentCount())
        pStart = 0;


    for (size_t i = 0; i < this->componentCount(); ++i)
    {
        if (areMatching(i, *this, pStart, pattern))
        {
            std::vector<uint8_t> visited(pattern.componentCount(), false);
            std::unordered_map<size_t, size_t> mapping;

            if (DFSCompare(i, *this, pStart, pattern, visited, mapping, escapeRadicalTypes) == false)
                continue;

            if (pattern.isCyclic() == false)
                return mapping;

            // re-verifying connectivity is necessary for cycles
            if(checkConnectivity(*this, pattern, mapping))
                return mapping;
        }
    }
    return std::unordered_map<size_t, size_t>();
}

std::unordered_map<size_t, size_t> MolecularStructure::maximalMapTo(const MolecularStructure& pattern) const
{
    if (pattern.componentCount() == 0 || this->componentCount() == 0)
        return std::unordered_map<size_t, size_t>();

    // find atom that matches in both target and pattern
    std::pair<std::unordered_map<size_t, size_t>, uint8_t> maxMapping;
    uint8_t maxScore = 0;
    for (size_t i = 0; i < this->componentCount(); ++i)
    {
        if (BaseComponent::areEqual(*this->components[i], *(pattern.components[0])) == false)
            continue;

        const auto score = getBondSimilarity(i, *this, 0, pattern);
        std::vector<uint8_t> visited(pattern.componentCount(), false);
        std::unordered_set<size_t> mappedA, mappedB;
        auto map = DFSMaximal(i, *this, mappedA, 0, pattern, mappedB);

        // picks largest mapping, then best 2nd comp. score, then best 1st comp. score
        if (map.first.size() > maxMapping.first.size() ||
            (map.first.size() == maxMapping.first.size() && (
                map.second > maxMapping.second ||
                (map.second == maxMapping.second &&
                    score > maxScore))))
        {
            maxMapping = std::move(map);
            maxScore == score;
        }
    }
    return maxMapping.first;
}

bool MolecularStructure::operator==(const MolecularStructure& other) const
{
    if (this->componentCount() != other.componentCount())
        return false;

    const auto mapping = this->mapTo(other, false);
    return mapping.size() == this->componentCount();
}

bool MolecularStructure::operator!=(const MolecularStructure& other) const
{
    if (this->componentCount() != other.componentCount())
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