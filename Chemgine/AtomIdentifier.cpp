#include "AtomIdentifier.hpp"

AtomIdentifier::AtomIdentifier(const CompositeComponent& composite, const AtomIdType id) :
	Atom(id),
	composite(composite)
{}