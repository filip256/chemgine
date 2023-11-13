#include "LabwareSystem.hpp"
#include "ContainerLabwareData.hpp"
#include "Maths.hpp"

PortIdentifier::PortIdentifier(
	LabwareSystem& system,
	const l_size componentIdx,
	const uint8_t portIdx
) noexcept :
	system(system),
	componentIdx(componentIdx),
	portIdx(portIdx)
{}

bool PortIdentifier::isValid() const
{
	return componentIdx != LabwareSystem::npos;
}

l_size PortIdentifier::getComponentIndex() const
{
	return componentIdx;
}

uint8_t PortIdentifier::getPortIndex() const
{
	return portIdx;
}

const BaseLabwareComponent& PortIdentifier::getComponent() const
{
	return *system.components[componentIdx];
}

LabwareSystem& PortIdentifier::getSystem()
{
	return system;
}

const LabwarePort* PortIdentifier::operator->() const
{
	return &system.components[componentIdx]->getPort(portIdx);
}

LabwareSystem::LabwareSystem(const BaseLabwareComponent& component) noexcept
{
	add(component);
}

l_size LabwareSystem::size() const
{
	return components.size();
}

void LabwareSystem::add(BaseLabwareComponent* component)
{
	components.emplace_back(component);

	const auto newBox = component->getSprite().getGlobalBounds();

	sf::Vector2f end(
		std::max(boundingBox.left + boundingBox.width, newBox.left + newBox.width),
		std::max(boundingBox.top + boundingBox.height, newBox.top + newBox.height)
	);

	boundingBox.left = std::min(boundingBox.left, newBox.left);
	boundingBox.top = std::min(boundingBox.top, newBox.top);
	boundingBox.width = end.x - boundingBox.left;
	boundingBox.height = end.y - boundingBox.top;
}

void LabwareSystem::add(BaseLabwareComponent* component, uint8_t componentPort, const PortIdentifier& thisPort)
{
	component->setRotation(thisPort->angle);

	const auto& cp = component->getPort(componentPort);
	component->setPosition(
		thisPort.getComponent().getSprite().getPosition() + sf::Vector2f(thisPort->x, thisPort->y) - sf::Vector2f(cp.x, cp.y) + component->getOrigin()
	);


	add(component);
}

void LabwareSystem::add(const BaseLabwareComponent& component)
{
	add(component.clone());
}

void LabwareSystem::draw(sf::RenderTarget& target) const
{
#ifndef NDEBUG
	sf::RectangleShape bBox(boundingBox.getSize());
	bBox.setPosition(boundingBox.getPosition());
	bBox.setFillColor(sf::Color(100, 255, 100, 50));
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

std::pair<PortIdentifier, float> LabwareSystem::findClosestPort(const sf::Vector2f& point, const float maxSqDistance)
{
	auto result = std::make_pair(PortIdentifier(*this, npos, 0), -1.0f);

	if (Maths::sqaredDistance(
		point.x, point.y,
		boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height) > maxSqDistance)
		return result;

	result.second = maxSqDistance;

	for (l_size i = 0; i < components.size(); ++i)
	{
		if (components[i]->isContainerType() == false)
			continue;

		const auto& ports = components[i]->getPorts();
		const auto& offset = components[i]->getPosition();

		for (uint8_t j = 0; j < ports.size(); ++j)
		{
			const float dist = Maths::sqaredDistance(point.x, point.y, ports[j].x + offset.x, ports[j].y + offset.y);
			if (dist <= result.second)
			{
				result.second = dist;
				result.first.componentIdx = i;
				result.first.portIdx = j;
			}
		}
	}

	return result;
}

std::pair<PortIdentifier, PortIdentifier> LabwareSystem::findClosestPort(LabwareSystem& other, const float maxSqDistance)
{
	auto result = std::make_pair(PortIdentifier(*this, npos, 0), PortIdentifier(other, npos, 0));

	float minDist = maxSqDistance;
	for (l_size i = 0; i < components.size(); ++i)
	{
		const auto& ports = components[i]->getPorts();
		for (uint8_t j = 0; j < ports.size(); ++j)
		{
			const auto temp =
				other.findClosestPort(components[i]->getPosition() + sf::Vector2f(ports[j].x, ports[j].y), maxSqDistance);

			if (temp.first.isValid() && temp.second < minDist)
			{
				minDist = temp.second;
				result.first.componentIdx = i;
				result.first.portIdx = j;
				result.second.componentIdx = temp.first.componentIdx;
				result.second.portIdx = temp.first.portIdx;
			}
		}
	}

	return result;
}

BaseLabwareComponent* LabwareSystem::release(const l_size componentIdx)
{
	return components.release(componentIdx);
}

void LabwareSystem::connect(PortIdentifier& destination, PortIdentifier& source)
{
	destination.getSystem().add(
		source.getSystem().release(source.getComponentIndex()),
		source.getPortIndex(),
		destination);
}