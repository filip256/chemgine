#include "Atom.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"


Atom::Atom(const ComponentIdType id) :
    AtomicComponent(ComponentType::ATOM),
    id(id)
{
    if (dataStore().atoms.contains(id) == false)
    {
        Logger::log("Atomic id " + std::to_string(id) + " is undefined.", LogType::BAD);
        this->id = 0;
    }
}

Atom::Atom(const std::string& symbol) :
    AtomicComponent(ComponentType::ATOM),
    id(0)
{
    if (dataStore().atoms.contains(symbol) == false)
    {
        Logger::log("Atomic symbol '" + symbol + "' is undefined.", LogType::BAD);
        return;
    }
    id = dataStore().atoms[symbol].id;
}

Atom::Atom(const char symbol) :
    Atom::Atom(std::string(1, symbol))
{}

const AtomData& Atom::data() const
{
    return dataStore().atoms[id];
}

bool Atom::isRadicalType() const
{
    return dataStore().atoms[id].weight == 0;
}

bool Atom::isDefined(const ComponentIdType id)
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