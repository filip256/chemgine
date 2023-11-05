#include "LabwareSystem.hpp"
#include "ContainerLabwareData.hpp"
#include "Maths.hpp"

LabwareSystem::LabwareSystem(const BaseLabwareComponent& component) noexcept
{
	add(component);
}

void LabwareSystem::add(const BaseLabwareComponent& component)
{
	components.emplace_back(component.clone());

	const auto newBox = component.getSprite().getGlobalBounds();

	if (newBox.left + newBox.width > boundingBox.left + boundingBox.width)
		boundingBox.width = newBox.left + newBox.width - boundingBox.left;
	if (newBox.top + newBox.height > boundingBox.top + boundingBox.height)
		boundingBox.height = newBox.top + newBox.height - boundingBox.top;

	if (newBox.left < boundingBox.left)
		boundingBox.left = newBox.left;
	if (newBox.top < boundingBox.top)
		boundingBox.top = newBox.top;
}

void LabwareSystem::draw(sf::RenderTarget& target) const
{
#ifndef NDEBUG
	sf::RectangleShape bBox(boundingBox.getSize());
	bBox.setPosition(boundingBox.getPosition());
	bBox.setFillColor(sf::Color(255, 255, 255, 50));
	target.draw(bBox);
#endif

	for (l_size i = 0; i < components.size(); ++i)
		components[i]->draw(target);
}

void LabwareSystem::move(const sf::Vector2f& offset)
{
	boundingBox.left += offset.x;
	boundingBox.top += offset.y;

	for (l_size i = 0; i < components.size(); ++i)
		components[i]->getSprite().move(offset);
}

l_size LabwareSystem::contains(const sf::Vector2f& point) const
{
	if (boundingBox.contains(point) == false)
		return npos;

	for (l_size i = 0; i < components.size(); ++i)
		if (components[i]->contains(point))
			return i;

	return npos;
}

bool LabwareSystem::intersects(const LabwareSystem& other) const
{
	if (this->boundingBox.intersects(other.boundingBox) == false)
		return false;

	for (l_size i = 0; i < this->components.size(); ++i)
		for (l_size j = 0; j < other.components.size(); ++j)
			if (this->components[i]->intersects(*other.components[j]))
				return true;

	return false;
}

l_size LabwareSystem::findFirst() const
{
	return components.empty() ? npos : 0;
}

PortIterator LabwareSystem::findClosestPort(const sf::Vector2f& point, const float maxSqDistance) const
{
	PortIterator result(npos, 0);

	if (Maths::sqaredDistance(
		point.x, point.y,
		boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height) > maxSqDistance)
		return result;

	float minDist = maxSqDistance;

	for (l_size i = 0; i < components.size(); ++i)
	{
		if (components[i]->isContainerType() == false)
			continue;

		const auto& joints = static_cast<const ContainerLabwareData&>(components[i]->getData()).joints;
		const auto& offset = components[i]->getSprite().getPosition();

		for (uint8_t j = 0; j < joints.size(); ++j)
		{
			const float dist = Maths::sqaredDistance(point.x, point.y, joints[j].x + offset.x, joints[j].y + offset.y);
			if (dist <= minDist)
			{
				minDist = dist;
				result.first = i;
				result.second = j;
			}
		}
	}

	return result;
}