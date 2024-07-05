#include "Atom.hpp"
#include "DataStore.hpp"
#include "Log.hpp"

size_t Atom::instanceCount = 0;

Atom::Atom(const AtomId id) noexcept :
    id(isDefined(id) ? id : 0)
{
    if (this->id == 0)
        Log(this).error("Atom id {0} is undefined.", id);
}

Atom::Atom(const Symbol symbol) noexcept :
    id(isDefined(symbol) ? dataStore().atoms.at(symbol).id : 0)
{
    if (id == 0)
        Log(this).error("Atom symbol '{0}' is undefined.", symbol.getAsString());
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

std::string Atom::getSymbol() const
{
    return data().symbol.getAsString();
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
