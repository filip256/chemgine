#include "AtomIdentifier.hpp"

AtomIdentifier::AtomIdentifier(const CompositeComponent& composite) :
	AtomicComponent(ComponentType::ATOM_ID),
	composite(composite)
{}

const Atom& AtomIdentifier::atom() const
{
	auto temp = composite.getComponent(ids[0]);
	size_t i = 1;
	for (size_t i = 1; i < ids.size(); ++i)
	{
		temp = static_cast<const CompositeComponent*>(temp)->getComponent(ids[i]);
	}
	return *static_cast<const Atom*>(temp);
}

const AtomData& AtomIdentifier::data() const
{
	return atom().data();
}

bool AtomIdentifier::isRadicalType() const
{
	return atom().isRadicalType();
}