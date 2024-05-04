#include "BaseContainerComponent.hpp"

BaseContainerComponent::BaseContainerComponent(
	const LabwareId id,
	const LabwareType type
) noexcept :
	DrawableComponent(id, type)
{}

const BaseContainerLabwareData& BaseContainerComponent::getData() const
{
	return static_cast<const BaseContainerLabwareData&>(data);
}

bool BaseContainerComponent::isContainer() const
{
	return true;
}

void BaseContainerComponent::setOverflowTarget(BaseContainerComponent& target, const OverflowTargetId id)
{
	setOverflowTarget(target.getContent(), id);
}
