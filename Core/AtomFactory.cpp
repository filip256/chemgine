#include "AtomFactory.hpp"
#include "Radical.hpp"

std::unique_ptr<const Atom> AtomFactory::get(const Symbol& symbol)
{
	return Radical::isDefined(symbol) ?
		std::make_unique<Radical>(symbol) :
		std::make_unique<Atom>(symbol);
}
