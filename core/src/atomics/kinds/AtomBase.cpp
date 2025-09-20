#include "atomics/kinds/AtomBase.hpp"

#include "data/DataStore.hpp"

AtomBase::AtomBase(const AtomBaseData& data) noexcept :
    data(data)
{}

AtomBase::AtomBase(const Symbol& symbol) noexcept :
    data(getDataStore().atoms.at(symbol))
{}

const AtomBaseData& AtomBase::getData() const { return data; }

bool AtomBase::isRadical() const { return data.isRadical(); }

uint8_t AtomBase::getPrecedence() const { return data.getPrecedence(); }

const Symbol& AtomBase::getSymbol() const { return data.symbol; }

std::string AtomBase::getSMILES() const { return data.getSMILES(); }

bool AtomBase::equals(const AtomBase& other) const { return &this->data == &other.data; }

bool AtomBase::isDefined(const Symbol& symbol) { return getDataStore().atoms.contains(symbol); }
