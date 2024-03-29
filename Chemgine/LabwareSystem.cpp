#include "LabwareSystem.hpp"
#include "ContainerLabwareData.hpp"
#include "Maths.hpp"
#include "ContainerComponent.hpp"
#include "Flask.hpp"

#include <queue>

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

BaseLabwareComponent& PortIdentifier::getComponent()
{
	return *system.components[componentIdx];
}

const LabwareSystem& PortIdentifier::getSystem() const
{
	return system;
}

LabwareSystem& PortIdentifier::getSystem()
{
	return system;
}

const DrawablePort* PortIdentifier::operator->() const
{
	return &system.components[componentIdx]->getPort(portIdx);
}



LabwareSystem::LabwareSystem(BaseLabwareComponent* component) noexcept
{
	add(component);
}

LabwareSystem::~LabwareSystem() noexcept
{
	while (components.size())
	{
		delete components.back();
		components.pop_back();
	}
}

l_size LabwareSystem::size() const
{
	return components.size();
}

const BaseLabwareComponent& LabwareSystem::getComponent(const size_t idx) const
{
	return *components[idx];
}

BaseLabwareComponent& LabwareSystem::getComponent(const size_t idx)
{
	return *components[idx];
}

void LabwareSystem::add(BaseLabwareComponent* component)
{
	components.emplace_back(component);
	connections.emplace_back(std::vector<LabwareConnection>(component->getPorts().size(), LabwareConnection(npos, 0, 0)));

	addToBoundry(component->getBounds());
}

void LabwareSystem::add(PortIdentifier& srcPort, PortIdentifier& destPort)
{
	LabwareSystem& dest = destPort.getSystem();
	LabwareSystem& src = srcPort.getSystem();

	if (auto flask = destPort.getComponent().as<Flask>())
	{
		if (auto target = srcPort.getComponent().as<BaseContainerComponent>())
			flask->setOverflowTarget(target.get());
	}
	else if (auto flask = srcPort.getComponent().as<Flask>())
	{
		if (auto target = destPort.getComponent().as<BaseContainerComponent>())
			flask->setOverflowTarget(target.get());
	}

	srcPort.getComponent().setRotation(destPort->angle - srcPort->angle + destPort.getComponent().getRotation());
	srcPort.getComponent().setPosition(destPort->position - srcPort->position + destPort.getComponent().getPosition());

	for (uint8_t i = 0; i < src.connections[srcPort.componentIdx].size(); ++i)
		if (src.connections[srcPort.componentIdx][i].isFree() == false)
			src.recomputePositions(srcPort.componentIdx, i);

	const l_size destSize = dest.size();
	const l_size srcSize = src.size();
	dest.components.reserve(destSize + srcSize);
	dest.connections.reserve(destSize + srcSize);

	while (src.size())
	{
		dest.components.emplace_back(src.components.back());
		src.components.pop_back();

		dest.addToBoundry(dest.components.back()->getBounds());

		dest.connections.emplace_back(std::move(src.connections.back()));
		src.connections.pop_back();

		for (uint8_t j = 0; j < dest.connections.back().size(); ++j)
			if (dest.connections.back()[j].isFree() == false)
			{
				dest.connections.back()[j].otherComponent =
					srcSize - dest.connections.back()[j].otherComponent - 1 + destSize;
			}
	}

	const auto translatedComponent = srcSize - srcPort.componentIdx - 1 + destSize;
	dest.connections[destPort.componentIdx][destPort.portIdx] = LabwareConnection(
		translatedComponent,
		srcPort.portIdx,
		LabwareConnection::getStrength(destPort->type, dest.components[translatedComponent]->getPort(srcPort.portIdx).type));

	dest.connections[translatedComponent][srcPort.portIdx] = LabwareConnection(
		destPort.componentIdx,
		destPort.portIdx,
		LabwareConnection::getStrength(dest.components[translatedComponent]->getPort(srcPort.portIdx).type, destPort->type));
}

void LabwareSystem::clearBoundry()
{
	boundingBox = sf::FloatRect(
		std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
}

void LabwareSystem::recomputeBoundry()
{
	clearBoundry();
	for (l_size i = 0; i < components.size(); ++i)
		addToBoundry(components[i]->getBounds());
}

void LabwareSystem::addToBoundry(const sf::FloatRect& box)
{
	sf::Vector2f end(
		std::max(boundingBox.left + boundingBox.width, box.left + box.width),
		std::max(boundingBox.top + boundingBox.height, box.top + box.height)
	);

	boundingBox.left = std::min(boundingBox.left, box.left);
	boundingBox.top = std::min(boundingBox.top, box.top);
	boundingBox.width = end.x - boundingBox.left;
	boundingBox.height = end.y - boundingBox.top;
}

void LabwareSystem::removeFromBoundry(const sf::FloatRect& box)
{
	if (isOnBoundry(box) == false)
		return;

	clearBoundry();
	for (l_size i = 0; i < components.size(); ++i)
		addToBoundry(components[i]->getBounds());
}

bool LabwareSystem::isOnBoundry(const sf::FloatRect& box) const
{
	return box.left <= boundingBox.left || box.top <= boundingBox.top ||
		box.left + box.width >= boundingBox.left + boundingBox.width ||
		box.top + box.height >= boundingBox.top + boundingBox.height;
}

void LabwareSystem::tick(const Amount<Unit::SECOND> timespan)
{
	for (l_size i = 0; i < components.size(); ++i)
		components[i]->tick(timespan);
}

void LabwareSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#ifndef NDEBUG
	sf::RectangleShape bBox(boundingBox.getSize());
	bBox.setPosition(boundingBox.getPosition());
	bBox.setFillColor(sf::Color(255, 255, 255, 25));
	target.draw(bBox);
#endif

	for (l_size i = 0; i < components.size(); ++i)
		target.draw(*components[i]);
}

void LabwareSystem::move(const sf::Vector2f& offset)
{
	boundingBox.left += offset.x;
	boundingBox.top += offset.y;

	for (l_size i = 0; i < components.size(); ++i)
		components[i]->move(offset);
}

void LabwareSystem::recomputePositions(const l_size parent, const uint8_t parentPort)
{
	const l_size current = connections[parent][parentPort].otherComponent;
	const uint8_t currentPort = connections[parent][parentPort].otherPort;

	components[current]->setRotation(
		components[parent]->getPort(parentPort).angle -
		components[current]->getPort(currentPort).angle +
		components[parent]->getRotation());

	components[current]->setPosition(
		components[parent]->getPosition() +
		components[parent]->getPort(parentPort).position -
		components[current]->getPort(currentPort).position
	);

	for (uint8_t i = 0; i < connections[current].size(); ++i)
		if (connections[current][i].isFree() == false && parent != connections[current][i].otherComponent)
			recomputePositions(current, i);
}

void LabwareSystem::rotate(const float angle, const l_size center)
{
	components[center]->setRotation(angle + components[center]->getRotation());
	for (uint8_t i = 0; i < connections[center].size(); ++i)
		if (connections[center][i].isFree() == false)
			recomputePositions(center, i);
	recomputeBoundry();
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

bool LabwareSystem::isFree(const l_size componentIdx, const uint8_t portIdx) const
{
	return connections[componentIdx][portIdx].isFree();
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
		const auto& ports = components[i]->getPorts();
		const auto& offset = components[i]->getPosition();

		for (uint8_t j = 0; j < ports.size(); ++j)
		{
			const float dist = Maths::sqaredDistance(point.x, point.y, ports[j].position.x + offset.x, ports[j].position.y + offset.y);
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
				other.findClosestPort(components[i]->getPosition() + ports[j].position, maxSqDistance);

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

BaseLabwareComponent* LabwareSystem::releaseComponent(const l_size componentIdx)
{
	if(components.size() == 1)
	{
		clearBoundry();
		const auto temp = components[componentIdx];
		components.erase(components.begin() + componentIdx);
		return temp;
	}

	// decrement connection ports before idx
	for (l_size i = 0; i < componentIdx; ++i)
	{
		for (auto& c : connections[i])
		{
			if (c.isFree())
				continue;

			if (c.otherComponent > componentIdx)
				c.otherComponent -= 1;
			else if (c.otherComponent == componentIdx)
				c.setFree();
		}
	}

	// move and decrement connection ports after idx
	for (l_size i = componentIdx; i + 1 < connections.size(); ++i)
	{
		for (auto& c : connections[i + 1])
		{
			if (c.isFree())
				continue;

			if (c.otherComponent > componentIdx)
				c.otherComponent -= 1;
			else if (c.otherComponent == componentIdx)
				c.setFree();
		}
		connections[i] = std::move(connections[i + 1]);
	}
	connections.pop_back();

	const auto freeComponent = components[componentIdx];
	components.erase(components.begin() + componentIdx);

	// maintain bounding box
	removeFromBoundry(freeComponent->getBounds());

	return freeComponent;
}

LabwareSystem LabwareSystem::releaseSection(const l_size componentIdx, const uint8_t portIdx, std::vector<l_size>& componentsToRemove)
{
	const auto first = this->connections[componentIdx][portIdx].otherComponent;
	this->connections[componentIdx][portIdx].setFree();

	LabwareSystem newSystem;
	std::queue<std::pair<l_size, l_size>> queue;

	// first component
	this->removeFromBoundry(this->components[first]->getBounds());
	newSystem.add(this->components[first]);
	newSystem.connections.emplace_back(std::move(this->connections[first]));
	componentsToRemove.emplace_back(first);

	for (uint8_t i = 0; i < newSystem.connections.back().size(); ++i)
		if(newSystem.connections.back()[i].isFree() == false)
		{
			if (newSystem.connections.back()[i].otherComponent == componentIdx)
				newSystem.connections.back()[i].setFree();
			else
			{
				queue.push(std::make_pair(newSystem.connections.back()[i].otherComponent, first));
				newSystem.connections.back()[i].otherComponent = i + 1;
			}
		}

	while (queue.size())
	{
		const auto& c = queue.front();

		newSystem.add(this->components[c.first]);
		newSystem.connections.emplace_back(std::move(this->connections[c.first]));
		componentsToRemove.emplace_back(c.first);

		for (uint8_t i = 0; i < newSystem.connections.back().size(); ++i)
			if (newSystem.connections.back()[i].isFree() == false &&
				newSystem.connections.back()[i].otherComponent != c.second)
			{
				queue.push(std::make_pair(newSystem.connections.back()[i].otherComponent, c.first));
				newSystem.connections.back()[i].otherComponent = i + newSystem.components.size();
			}

		queue.pop();
	}

	return newSystem;
}

bool LabwareSystem::isFree(const PortIdentifier& port)
{
	return port.getSystem().isFree(port.getComponentIndex(), port.getPortIndex());
}

void LabwareSystem::connect(PortIdentifier& destination, PortIdentifier& source)
{
	add(source, destination);
}

bool LabwareSystem::canConnect(const PortIdentifier& destination, const PortIdentifier& source)
{
	return
		isFree(destination) && isFree(source) &&
		LabwareConnection::getStrength(source->type, destination->type);
}

std::vector<LabwareSystem> LabwareSystem::disconnect(const l_size componentIdx)
{
	std::vector<LabwareSystem> result;

	l_size diff = 0;
	sf::Vector2f push(0.0f, 0.0f);
	if (components.size() > 2)
	{
		std::vector<l_size> toRemove;
		bool skippedFirst = false;
		for (uint8_t i = 0; i < connections[componentIdx].size(); ++i)
		{
			const auto& c = connections[componentIdx][i];
			if (c.isFree())
				continue;

			const float angle = Maths::toRadians(
				components[c.otherComponent - diff]->getPort(c.otherPort).angle +
				components[c.otherComponent - diff]->getRotation() - 90.0f);

			push.x += (components[componentIdx]->getPosition().x < components[c.otherComponent - diff]->getPosition().x ?
				1.0f : -1.0f) * std::cosf(angle);
			push.y += (components[componentIdx]->getPosition().y < components[c.otherComponent - diff]->getPosition().y ?
				1.0f : -1.0f) * std::sinf(angle);

			// one of the sub-sections can remain in this 
			if (skippedFirst == false)
			{
				skippedFirst = true;
				continue;
			}
			result.emplace_back(std::move(releaseSection(componentIdx, i, toRemove)));
			if (result.back().size() == 1)
				result.back().rotate(0.0f);
		}

		// components and connections are moved away but cannot be erased until the end because of indexes
		for (l_size i = 0; i < toRemove.size(); ++i)
		{
			if (toRemove[i] < componentIdx)
				++diff;
			components.erase(components.begin() + toRemove[i]);
			connections.erase(connections.begin() + toRemove[i]);
		}
	}
	else
	{
		for (auto& c : connections[componentIdx - diff])
		{
			if (c.isFree())
				continue;

			const float angle = Maths::toRadians(
				components[c.otherComponent - diff]->getPort(c.otherPort).angle +
				components[c.otherComponent - diff]->getRotation() - 90.0f);

			push.x += (components[componentIdx]->getPosition().x < components[c.otherComponent - diff]->getPosition().x ?
				1.0f : -1.0f) * std::cosf(angle);
			push.y += (components[componentIdx]->getPosition().y < components[c.otherComponent - diff]->getPosition().y ?
				1.0f : -1.0f) * std::sinf(angle);
			break;
		}
	}

	push.x = push.x * 20.0f;
	push.y = push.y * 20.0f;

	auto temp = releaseComponent(componentIdx - diff);
	temp->setRotation(0.0f);
	temp->move(push);

	if (components.size() == 1)
	{
		components.back()->setRotation(0.0f);
		boundingBox = components.back()->getBounds();
	}

	result.emplace_back(std::move(LabwareSystem(temp)));
	return result;
}
