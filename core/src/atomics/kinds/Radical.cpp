#include "atomics/kinds/Radical.hpp"

#include "data/DataStore.hpp"
#include "io/Log.hpp"

const RadicalData& Radical::getData() const { return static_cast<const RadicalData&>(data); }

bool Radical::matches(const AtomBase& other) const
{
    if (equals(other))
        return true;

    const auto& thisData = this->getData();
    return thisData.getMatches().contains(other.getSymbol()) || thisData.getMatches() == RadicalData::MatchAny;
}

std::optional<Radical> Radical::fromSymbol(const Symbol& symbol)
{
    const auto data = Accessor<>::getDataStore().atoms.find(symbol);
    return data != nullptr && data->isRadical() ? std::optional(Radical(*data)) : std::nullopt;
}
