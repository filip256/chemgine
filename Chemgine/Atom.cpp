#include "Atom.hpp"
#include "DataStore.hpp"
#include "Log.hpp"

size_t Atom::instanceCount = 0;

Atom::Atom(const Symbol symbol) noexcept :
    data(dataStore().atoms.at(symbol))
{}

const AtomData& Atom::getData() const
{
    return data;
}

bool Atom::isRadical() const
{
    return data.isRadical();
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
    
    return data.getRarity();
}

std::string Atom::getSymbol() const
{
    return data.symbol.getAsString();
}

std::string Atom::getSMILES() const
{
    return data.getSMILES();
}

std::unordered_map<Symbol, c_size> Atom::getComponentCountMap() const
{
    return std::unordered_map<Symbol, c_size> {std::make_pair(data.symbol, 1)};
}

bool Atom::operator==(const Atom& other) const
{
    return &this->data == &other.data;
}

bool Atom::operator!=(const Atom& other) const
{
    return &this->data != &other.data;
}

Atom* Atom::clone() const
{
    return new Atom(*this);
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
