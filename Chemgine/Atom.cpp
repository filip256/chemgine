#include "Atom.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"


Atom::Atom(const ComponentId id) :
    BaseComponent(id, ComponentType::ATOMIC)
{
    if (dataStore().atoms.contains(id) == false)
    {
        Logger::log("Atom id " + std::to_string(id) + " is undefined.", LogType::BAD);
        this->id = 0;
    }
}

Atom::Atom(const std::string& symbol) :
    BaseComponent(0 ,ComponentType::ATOMIC)
{
    if (dataStore().atoms.contains(symbol) == false)
    {
        Logger::log("Atom symbol '" + symbol + "' is undefined.", LogType::BAD);
        return;
    }
    id = dataStore().atoms.at(symbol).id;
}

Atom::Atom(const char symbol) :
    Atom::Atom(std::string(1, symbol))
{}

const AtomData& Atom::data() const
{
    return dataStore().atoms.at(id);
}

bool Atom::isRadicalType() const
{
    return dataStore().atoms.at(id).weight == 0;
}

bool Atom::isDefined(const ComponentId id)
{
    return getDataStore().atoms.contains(id);
}

bool Atom::isDefined(const std::string& symbol)
{
    return getDataStore().atoms.contains(symbol);
}

bool Atom::isDefined(const char symbol)
{
    return isDefined(std::string(1, symbol));
}

uint8_t Atom::getPrecedence() const
{
    if (isRadicalType())
        return 0;
    
    return data().getRarity();
}

std::unordered_map<ComponentId, c_size> Atom::getComponentCountMap() const
{
    return std::unordered_map<ComponentId, c_size> {std::make_pair(id, 1)};
}

Atom* Atom::clone() const
{
    return new Atom(*this);
}
