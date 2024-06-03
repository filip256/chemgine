#include "Atom.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

size_t Atom::instanceCount = 0;

Atom::Atom(const AtomId id) noexcept :
    id(isDefined(id) ? id : 0)
{
    if (this->id == 0)
        Logger::log("Atom id " + std::to_string(id) + " is undefined.", LogType::BAD);
}

Atom::Atom(const Symbol symbol) noexcept :
    id(isDefined(symbol) ? dataStore().atoms.at(symbol).id : 0)
{
    if (id == 0)
        Logger::log("Atom symbol '" + symbol.getAsString() + "' is undefined.", LogType::BAD);
}

const AtomData& Atom::data() const
{
    return dataStore().atoms.at(id);
}

bool Atom::isRadical() const
{
    return data().isRadical();
}

bool Atom::equals(const Atom& other) const
{
    return *this == other;
}

bool Atom::matches(const Atom& other) const
{
    return *this == other;
}

uint8_t Atom::getPrecedence() const
{
    if (isRadical())
        return 0;
    
    return data().getRarity();
}

std::string Atom::getSMILES() const
{
    return data().getSMILES();
}

std::unordered_map<AtomId, c_size> Atom::getComponentCountMap() const
{
    return std::unordered_map<AtomId, c_size> {std::make_pair(id, 1)};
}

bool Atom::operator==(const Atom& other) const
{
    return this->id == other.id;
}

bool Atom::operator!=(const Atom& other) const
{
    return this->id != other.id;
}

Atom* Atom::clone() const
{
    return new Atom(*this);
}

bool Atom::isDefined(const AtomId id)
{
    return getDataStore().atoms.contains(id);
}

bool Atom::isDefined(const Symbol symbol)
{
    return getDataStore().atoms.contains(symbol);
}



#ifndef NDEBUG
void* Atom::operator new(const size_t count)
{
    ++instanceCount;
    return ::operator new(count);
}

void Atom::operator delete(void* ptr)
{
    --instanceCount;
    return ::operator delete(ptr);
}
#endif
