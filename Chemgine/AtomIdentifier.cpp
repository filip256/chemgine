#include "AtomIdentifier.hpp"

AtomIdentifier::AtomIdentifier(const CompositeComponent& composite) :
	BaseComponent(ComponentType::ATOM_ID),
	composite(composite)
{}

const AtomData& AtomIdentifier::data() const
{
	auto temp = composite.getComponent(ids[0]);
	size_t i = 1;
	for(size_t i = 1; i < ids.size(); ++i)
	{
		temp = static_cast<const CompositeComponent*>(temp)->getComponent(ids[i]);
	}
	return temp->data();
}