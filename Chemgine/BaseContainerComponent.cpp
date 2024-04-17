#include "BaseContainerComponent.hpp"

BaseContainerComponent::BaseContainerComponent(const LabwareId id) noexcept :
	DrawableComponent(id)
{}

const BaseContainerLabwareData& BaseContainerComponent::getData() const
{
	return static_cast<const BaseContainerLabwareData&>(data);
}

bool BaseContainerComponent::isContainer() const
{
	return true;
}

void BaseContainerComponent::setOverflowTarget(BaseContainerComponent& target)
{
	setOverflowTarget(target.getContent());
}
