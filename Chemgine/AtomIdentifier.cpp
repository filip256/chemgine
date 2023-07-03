#include "AtomIdentifier.hpp"

AtomIdentifier::AtomIdentifier(const CompositeComponent& composite) :
	AtomicComponent(0, ComponentType::ATOM_ID),
	composite(composite)
{}

ComponentIdType AtomIdentifier::atom() const
{
	if (id != 0)
		return id;

	auto temp = composite.getComponent(ids[0]);
	size_t i = 1;
	for (size_t i = 1; i < ids.size(); ++i)
	{
		temp = static_cast<const CompositeComponent*>(temp)->getComponent(ids[i]);
	}
	id = temp->getId();
	return id;
}

const AtomData& AtomIdentifier::data() const
{
	return Atom(id).data();
}

bool AtomIdentifier::isRadicalType() const
{
	return Atom(id).isRadicalType();
}