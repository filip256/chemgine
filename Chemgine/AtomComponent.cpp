#include "AtomComponent.hpp"

AtomComponent::AtomComponent(const CompositeComponent& composite, const AtomIdType id) :
	Atom(id),
	composite(composite)
{}