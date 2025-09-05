#include "atomics/kinds/Atom.hpp"

#include "data/DataStore.hpp"
#include "io/Log.hpp"

Atom::Atom(const Symbol& symbol) noexcept :
    data(Accessor<>::getDataStore().atoms.at(symbol))
{}

const AtomData& Atom::getData() const { return data; }

bool Atom::isRadical() const { return data.isRadical(); }

bool Atom::equals(const Atom& other) const { return *this == other; }

bool Atom::matches(const Atom& other) const { return *this == other; }

uint8_t Atom::getPrecedence() const
{
    if (isRadical())
        return 0;

    return data.getRarity();
}

const Symbol& Atom::getSymbol() const { return data.symbol; }

std::string Atom::getSMILES() const { return data.getSMILES(); }

std::unordered_map<Symbol, c_size> Atom::getComponentCountMap() const
{
    return std::unordered_map<Symbol, c_size>{std::make_pair(data.symbol, 1)};
}

bool Atom::operator==(const Atom& other) const { return this->data.symbol == other.data.symbol; }

bool Atom::operator!=(const Atom& other) const { return this->data.symbol != other.data.symbol; }

std::unique_ptr<Atom> Atom::clone() const { return std::make_unique<Atom>(*this); }

bool Atom::isDefined(const Symbol& symbol) { return Accessor<>::getDataStore().atoms.contains(symbol); }
