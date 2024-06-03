#include "Radical.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

Radical::Radical(const AtomId id) noexcept :
	Atom(id)
{
	if(isRadical() == false)
		Logger::log("Radical initialized with non-radical atom data.", LogType::BAD);
}

Radical::Radical(const Symbol symbol) noexcept :
	Atom(symbol)
{
	if (isRadical() == false)
		Logger::log("Radical initialized with non-radical atom data.", LogType::BAD);
}

const RadicalData& Radical::data() const
{
	return static_cast<const RadicalData&>(dataStore().atoms.at(id));
}

bool Radical::matches(const Atom& other) const
{
	return equals(other) ||
		data().matchables == RadicalData::MatchAny ||
		data().matchables.contains(other.id);
}

Radical* Radical::clone() const
{
	return new Radical(*this);
}

bool Radical::isDefined(const AtomId id)
{
	return Atom::isDefined(id) && getDataStore().atoms.at(id).isRadical();
}

bool Radical::isDefined(const Symbol symbol)
{
	return Atom::isDefined(symbol) && getDataStore().atoms.at(symbol).isRadical();
}
