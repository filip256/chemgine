#include "CompositeComponent.hpp"

CompositeComponent::CompositeComponent(const ComponentType type) :
	BaseComponent(type),
	structure("")
{}

const BaseComponent* CompositeComponent::getComponent(const size_t idx) const
{
	return structure.getComponent(idx);
}