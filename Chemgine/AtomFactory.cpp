#include "AtomFactory.hpp"
#include "Radical.hpp"

Atom* AtomFactory::get(const AtomId id)
{
	return Radical::isDefined(id) ?
		new Radical(id) :
		new Atom(id);
}

Atom* AtomFactory::get(const Symbol symbol)
{
	return Radical::isDefined(symbol) ?
		new Radical(symbol) :
		new Atom(symbol);
}
