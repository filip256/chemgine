#include "BaseContainerComponent.hpp"

BaseContainerComponent::BaseContainerComponent(const LabwareId id) noexcept :
	DrawableComponent(id),
	fill(getData().fillTexture, getData().textureScale)
{}

const ContainerLabwareData& BaseContainerComponent::getData() const
{
	return static_cast<const ContainerLabwareData&>(data);
}

bool BaseContainerComponent::isContainerType() const
{
	return true;
}

void BaseContainerComponent::setPosition(const sf::Vector2f& position)
{
	fill.setPosition(position);
	DrawableComponent::setPosition(position);
}

void BaseContainerComponent::move(const sf::Vector2f& offset)
{
	fill.move(offset);
	DrawableComponent::move(offset);
}

void BaseContainerComponent::setRotation(const float angle)
{
	fill.setRotation(angle);
	DrawableComponent::setRotation(angle);
}

void BaseContainerComponent::setOverflowTarget(BaseContainerComponent& target)
{
	setOverflowTarget(target.getContent());
}
