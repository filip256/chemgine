#include "Atom.hpp"
#include "Logger.hpp"


Atom::Atom(const AtomIdType id, const ComponentType type) :
    BaseComponent(type),
    id(id)
{
    if (dataStore->atoms.contains(id) == false)
    {
        Logger::log("Atomic id " + std::to_string(id) + " is undefined.", LogType::BAD);
        this->id = 0;
    }
}

Atom::Atom(const std::string& symbol) :
    BaseComponent(ComponentType::ATOM)
{
    if (dataStore->atoms.contains(symbol) == false)
    {
        Logger::log("Atomic symbol '" + symbol + "' is undefined.", LogType::BAD);
        return;
    }
    id = dataStore->atoms[symbol].id;
}

Atom::Atom(const char symbol) :
    Atom::Atom(std::string(1, symbol))
{}

const AtomData& Atom::data() const
{
    return dataStore->atoms[id];
}

bool Atom::isDefined(const AtomIdType id)
{
    return dataStore->atoms.contains(id);
}

bool Atom::isDefined(const std::string& symbol)
{
    return dataStore->atoms.contains(symbol);
}

bool Atom::isDefined(const char symbol)
{
    return isDefined(std::string(1, symbol));
}