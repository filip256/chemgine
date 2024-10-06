#include "Radical.hpp"
#include "DataStore.hpp"
#include "Log.hpp"

Radical::Radical(const Symbol& symbol) noexcept :
	Atom(symbol)
{}

const RadicalData& Radical::getData() const
{
	return static_cast<const RadicalData&>(data);
}

bool Radical::matches(const Atom& other) const
{
	const auto& d = this->getData();
	return equals(other) ||
		d.matchables == RadicalData::MatchAny ||
		d.matchables.contains(other.getData().symbol);
}

Radical* Radical::clone() const
{
	return new Radical(*this);
}

bool Radical::isDefined(const Symbol& symbol)
{
	return Atom::isDefined(symbol) && getDataStore().atoms.at(symbol).isRadical();
}
