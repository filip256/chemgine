#include "CompositeComponent.hpp"

CompositeComponent::CompositeComponent(const ComponentId id, const ComponentType type) :
	BaseComponent(id, type)
{}

bool CompositeComponent::isRadicalType() const
{
	return false;
}