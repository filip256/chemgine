#include "atomics/kinds/Atom.hpp"

#include "data/DataStore.hpp"
#include "io/Log.hpp"

const AtomData& Atom::getData() const { return static_cast<const AtomData&>(data); }

bool Atom::matches(const AtomBase& other) const { return this->equals(other); }

std::optional<Atom> Atom::fromSymbol(const Symbol& symbol)
{
    const auto data = Accessor<>::getDataStore().atoms.find(symbol);
    return data != nullptr && not data->isRadical() ? std::optional(Atom(*data)) : std::nullopt;
}
