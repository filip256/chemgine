#include <queue>

#include "MolecularStructure.hpp"

MolecularStructure::MolecularStructure(const std::string& smiles)
{
    loadFromSMILES(smiles);
}

bool MolecularStructure::loadFromSMILES(const std::string& smiles)
{
    size_t i = 0;
    std::unordered_map<uint8_t, size_t> rings;
    std::stack<size_t> branches;

    BondType bondType = BondType::SINGLE;
    for(size_t i = 0; i < smiles.size(); ++i)
    {
        size_t prev = std::string::npos;

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
            branches.emplace(components.size() - 1);
            continue;
        }

        if (smiles[i] == ')')
        {
            prev = branches.top();
            branches.pop();
            continue;
        }

        if (smiles[i] == '[') // special atom identification
        {
            const size_t t = smiles.find(']', i + 1);
            if (t == std::string::npos)
                return false;

            components.emplace_back(new Atom(smiles.substr(i + 1, t - i - 1)));
            bonds.emplace_back(std::move(std::vector<Bond>()));

            if (prev != std::string::npos)
            {
                bonds[prev].emplace_back(std::move(Bond(components.size() - 1, bondType)));
                bonds[bonds.size() - 1].emplace_back(std::move(Bond(prev, bondType)));
                bondType = BondType::SINGLE;
            }

            prev = components.size() - 1;
            i = t + 1;
            continue;
        }

        if (smiles[i] == '%')
        {
            if (i + 2 >= smiles.size())
                return false;

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
    }

    return true;
}

const BaseComponent* MolecularStructure::getComponent(const size_t idx) const
{
    return components[idx];
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

    buffer[y][x] = components[c]->data().symbol[0];

    std::vector<size_t> unvisited;
    for (size_t i = 0; i < bonds[c].size(); ++i)
        if (visited[bonds[c][i].other] == false)
            unvisited.emplace_back(bonds[c][i].other);

    if (unvisited.size() >= 1)
    {
        buffer[y][x + 1] = 'Ä';
        rPrint(buffer, x + 2, y, unvisited[0], visited);
    }

    if (unvisited.size() >= 2)
    {
        buffer[y - 1][x] = '³';
        rPrint(buffer, x, y - 2, unvisited[1], visited);
    }

    if (unvisited.size() >= 3)
    {
        buffer[y][x - 1] = 'Ä';
        rPrint(buffer, x - 2, y, unvisited[2], visited);
    }

    if (unvisited.size() >= 4)
    {
        buffer[y + 1][x] = '³';
        rPrint(buffer, x, y + 2, unvisited[3], visited);
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