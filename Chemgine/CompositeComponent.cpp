#include "CompositeComponent.hpp"

CompositeComponent::CompositeComponent(const ComponentIdType id, const ComponentType type) :
	BaseComponent(id, type)
{}

bool CompositeComponent::isRadicalType() const
{
	return false;
}