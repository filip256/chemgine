#include "Atom.hpp"
#include "DataStore.hpp"
#include "Log.hpp"

Atom::Atom(const Symbol& symbol) noexcept :
    data(Accessor<>::getDataStore().atoms.at(symbol))
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
    return data.symbol.getString();
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

std::unique_ptr<Atom> Atom::clone() const
{
    return std::make_unique<Atom>(*this);
}

bool Atom::isDefined(const Symbol& symbol)
{
    return Accessor<>::getDataStore().atoms.contains(symbol);
}
