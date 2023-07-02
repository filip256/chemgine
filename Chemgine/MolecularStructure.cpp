#include <queue>

#include "MolecularStructure.hpp"
#include "CompositeComponent.hpp"
#include "Logger.hpp"

MolecularStructure::MolecularStructure(const std::string& smiles)
{
    loadFromSMILES(smiles);
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

            bonds.emplace_back(std::move(std::vector<Bond>()));

            if (prev != std::string::npos)
            {
                bonds[prev].emplace_back(std::move(Bond(components.size() - 1, bondType)));
                bonds[bonds.size() - 1].emplace_back(std::move(Bond(prev, bondType)));
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
            bonds.emplace_back(std::move(std::vector<Bond>()));

            if (prev != std::string::npos)
            {
                bonds[prev].emplace_back(std::move(Bond(components.size() - 1, bondType)));
                bonds[bonds.size() - 1].emplace_back(std::move(Bond(prev, bondType)));
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
                bonds[rings[label]].emplace_back(std::move(Bond(components.size() - 1, bondType)));
                bonds[components.size() - 1].emplace_back(std::move(Bond(rings[label], bondType)));
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
                bonds[rings[label]].emplace_back(std::move(Bond(components.size() - 1, bondType)));
                bonds[components.size() - 1].emplace_back(std::move(Bond(rings[label], bondType)));
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

    Logger::log("Molecular structure read successfully.", LogType::GOOD);

    return true;
}

int16_t MolecularStructure::getHCount() const
{
    int16_t hCount = 0;
    for (size_t i = 0; i < components.size(); ++i)
    {
        auto v = components[i]->data().valence;
        if (bonds[i].size() > v)
            return -1;

        uint8_t cnt = 0;
        for (size_t j = 0; j < bonds[i].size(); ++j)
            cnt += bonds[i][j].getValence();
        if (cnt > v)
            return -1;

        hCount += v - cnt;
    }
    return hCount;
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
            static_cast<AtomicComponent*>(components[i])->isRadicalType())
            ++cnt;
    return cnt;
}

bool MolecularStructure::isComplete() const
{
    // it does not search sub components
    for (size_t i = 0; i < components.size(); ++i)
        if (components[i]->isAtomicType() &&
            static_cast<AtomicComponent*>(components[i])->isRadicalType())
                return false;
    return true;
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
        if (visited[bonds[c][i].other] == false)
        {
            char vC = '³', hC = 'Ä';
            if (bonds[c][i].type == BondType::DOUBLE)
                vC = 'º', hC = 'Í';
            else if (bonds[c][i].type == BondType::TRIPLE)
                vC = 'ð', hC = 'ð';


            if (buffer[y][x + 2] == ' ')
            {
                buffer[y][x + 1] = hC;
                rPrint(buffer, x + 2, y, bonds[c][i].other, visited);
            }
            else if (buffer[y - 2][x] == ' ')
            {
                buffer[y - 1][x] = vC;
                rPrint(buffer, x, y - 2, bonds[c][i].other, visited);
            }
            else if (buffer[y][x - 2] == ' ')
            {
                buffer[y][x - 1] = hC;
                rPrint(buffer, x - 2, y, bonds[c][i].other, visited);
            }
            else if (buffer[y + 2][x] == ' ')
            {
                buffer[y + 1][x] = vC;
                rPrint(buffer, x, y + 2, bonds[c][i].other, visited);
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
        delete components.back();
        components.pop_back();
    }
    bonds.clear();
    hydrogenCount = 0;
}