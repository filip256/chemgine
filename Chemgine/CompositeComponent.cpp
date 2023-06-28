#include "CompositeComponent.hpp"

CompositeComponent::CompositeComponent(
	const std::string& smiles,
	const ComponentType type
) :
	BaseComponent(type),
	structure(smiles)
{}

const BaseComponent* CompositeComponent::getComponent(const size_t idx) const
{
	return structure.getComponent(idx);
}