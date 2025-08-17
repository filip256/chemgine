#include "labware/LabwareSystem.hpp"

#include "labware/kinds/ContainerComponent.hpp"
#include "labware/kinds/Flask.hpp"
#include "labware/kinds/Heatsource.hpp"
#include "labware/Lab.hpp"

#include <queue>

PortIdentifier::PortIdentifier(
	LabwareSystem& system,
	const l_size componentIdx,
	const uint8_t portIdx
) noexcept :
	componentIdx(componentIdx),
	portIdx(portIdx),
	system(system)
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

const LabwareComponentBase& PortIdentifier::getComponent() const
{
	return *system.components[componentIdx];
}

LabwareComponentBase& PortIdentifier::getComponent()
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



LabwareSystem::LabwareSystem(Lab& lab) noexcept :
	lab(lab)
{}

LabwareSystem::LabwareSystem(std::unique_ptr<LabwareComponentBase>&& component, Lab& lab) noexcept :
	LabwareSystem(lab)
{
	add(std::move(component));
}

l_size LabwareSystem::size() const
{
	return checked_cast<l_size>(components.size());
}

const LabwareComponentBase& LabwareSystem::getComponent(const size_t idx) const
{
	return *components[idx];
}

LabwareComponentBase& LabwareSystem::getComponent(const size_t idx)
{
	return *components[idx];
}

void LabwareSystem::add(std::unique_ptr<LabwareComponentBase>&& component)
{
	addToBoundary(component->getBounds());

	connections.emplace_back(std::vector<LabwareConnection>(component->getPorts().size(), LabwareConnection(npos, 0, 0)));
	components.emplace_back(std::move(component));
}

void LabwareSystem::add(PortIdentifier& srcPort, PortIdentifier& destPort)
{
	auto& srcSys = srcPort.getSystem();
	auto& srcComp = srcPort.getComponent();
	auto& destSys = destPort.getSystem();
	auto& destComp = destPort.getComponent();

	bool connectionSuccess = srcComp.tryConnect(destComp);
	connectionSuccess |= destComp.tryConnect(srcComp);   // make sure both are called
	if (connectionSuccess == false)
		Log<LabwareSystem>().warn("Port-supported connection resulted in no connection between components.");

	srcComp.setRotation(destPort->angle - srcPort->angle + destPort.getComponent().getRotation());
	srcComp.setPosition(destPort->position - srcPort->position + destPort.getComponent().getPosition());

	for (uint8_t i = 0; i < srcSys.connections[srcPort.componentIdx].size(); ++i)
		if (srcSys.connections[srcPort.componentIdx][i].isFree() == false)
			srcSys.recomputePositions(srcPort.componentIdx, i);

	const l_size destSize = destSys.size();
	const l_size srcSize = srcSys.size();
	destSys.components.reserve(destSize + srcSize);
	destSys.connections.reserve(destSize + srcSize);

	while (srcSys.size())
	{
		destSys.components.emplace_back(std::move(srcSys.components.back()));
		srcSys.components.pop_back();

		destSys.addToBoundary(destSys.components.back()->getBounds());

		destSys.connections.emplace_back(std::move(srcSys.connections.back()));
		srcSys.connections.pop_back();

		for (uint8_t j = 0; j < destSys.connections.back().size(); ++j)
			if (destSys.connections.back()[j].isFree() == false)
			{
				destSys.connections.back()[j].otherComponent =
					srcSize - destSys.connections.back()[j].otherComponent - 1 + destSize;
			}
	}

	const auto translatedComponent = srcSize - srcPort.componentIdx - 1 + destSize;
	destSys.connections[destPort.componentIdx][destPort.portIdx] = LabwareConnection(
		translatedComponent,
		srcPort.portIdx,
		LabwareConnection::getStrength(destPort->type, destSys.components[translatedComponent]->getPort(srcPort.portIdx).type));

	destSys.connections[translatedComponent][srcPort.portIdx] = LabwareConnection(
		destPort.componentIdx,
		destPort.portIdx,
		LabwareConnection::getStrength(destSys.components[translatedComponent]->getPort(srcPort.portIdx).type, destPort->type));
}

void LabwareSystem::clearBoundary()
{
	boundingBox = sf::FloatRect(
		sf::Vector2f(std::numeric_limits<float_s>::max(), std::numeric_limits<float_s>::max()),
		sf::Vector2f(-std::numeric_limits<float_s>::max(), -std::numeric_limits<float_s>::max()));
}

void LabwareSystem::recomputeBoundary()
{
	clearBoundary();
	for (l_size i = 0; i < components.size(); ++i)
		addToBoundary(components[i]->getBounds());
}

void LabwareSystem::addToBoundary(const sf::FloatRect& box)
{
	sf::Vector2f end(
		std::max(boundingBox.position.x + boundingBox.size.x, box.position.x + box.size.x),
		std::max(boundingBox.position.y + boundingBox.size.y, box.position.y + box.size.y)
	);

	boundingBox.position.x = std::min(boundingBox.position.x, box.position.x);
	boundingBox.position.y = std::min(boundingBox.position.y, box.position.y);
	boundingBox.size.x = end.x - boundingBox.position.x;
	boundingBox.size.y = end.y - boundingBox.position.y;
}

void LabwareSystem::removeFromBoundary(const sf::FloatRect& box)
{
	if (isOnBoundary(box) == false)
		return;

	clearBoundary();
	for (l_size i = 0; i < components.size(); ++i)
		addToBoundary(components[i]->getBounds());
}

bool LabwareSystem::isOnBoundary(const sf::FloatRect& box) const
{
	return box.position.x <= boundingBox.position.x || box.position.y <= boundingBox.position.y ||
		box.position.x + box.size.x >= boundingBox.position.x + boundingBox.size.x ||
		box.position.y + box.size.y >= boundingBox.position.y + boundingBox.size.y;
}

void LabwareSystem::tick(const Amount<Unit::SECOND> timespan)
{
	for (l_size i = 0; i < components.size(); ++i)
		components[i]->tick(timespan);
}

void LabwareSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#ifndef NDEBUG
	/*sf::RectangleShape bBox(boundingBox.getSize());
	bBox.setPosition(boundingBox.getPosition());
	bBox.setFillColor(sf::Color(255, 255, 255, 25));
	target.draw(bBox, states);*/
#endif

	for (l_size i = 0; i < components.size(); ++i)
		target.draw(*components[i], states);
}

void LabwareSystem::move(const sf::Vector2f& offset)
{
	boundingBox.position.x += offset.x;
	boundingBox.position.y += offset.y;

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

void LabwareSystem::rotate(const Amount<Unit::DEGREE> angle, const l_size center)
{
	components[center]->setRotation(angle + components[center]->getRotation());
	for (uint8_t i = 0; i < connections[center].size(); ++i)
		if (connections[center][i].isFree() == false)
			recomputePositions(center, i);
	recomputeBoundary();
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
	if (not this->boundingBox.findIntersection(other.boundingBox))
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

std::pair<PortIdentifier, float_s> LabwareSystem::findClosestPort(const sf::Vector2f& point, const float_s maxSqDistance)
{
	auto result = std::make_pair(PortIdentifier(*this, npos, 0), -1.0f);

	if (utils::squaredDistance(
		point.x, point.y,
		boundingBox.position.x, boundingBox.position.y, boundingBox.position.x + boundingBox.size.x, boundingBox.position.y + boundingBox.size.y) > maxSqDistance)
		return result;

	result.second = maxSqDistance;

	for (l_size i = 0; i < components.size(); ++i)
	{
		const auto& ports = components[i]->getPorts();
		const auto& offset = components[i]->getPosition();

		for (uint8_t j = 0; j < ports.size(); ++j)
		{
			const float_s dist = utils::squaredDistance(point.x, point.y, ports[j].position.x + offset.x, ports[j].position.y + offset.y);
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

std::pair<PortIdentifier, PortIdentifier> LabwareSystem::findClosestPort(LabwareSystem& other, const float_s maxSqDistance)
{
	auto result = std::make_pair(PortIdentifier(*this, npos, 0), PortIdentifier(other, npos, 0));

	float_s minDist = maxSqDistance;
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

std::unique_ptr<LabwareComponentBase> LabwareSystem::releaseComponent(const l_size componentIdx)
{
	if(components.size() == 1)
	{
		clearBoundary();
		auto temp = std::move(components[componentIdx]);
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
	for (l_size i = componentIdx; i + 1 < static_cast<l_size>(connections.size()); ++i)
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

	auto freeComponent = std::move(components[componentIdx]);
	components.erase(components.begin() + componentIdx);

	// maintain bounding box
	removeFromBoundary(freeComponent->getBounds());

	return freeComponent;
}

LabwareSystem LabwareSystem::releaseSection(const l_size componentIdx, const uint8_t portIdx, std::vector<l_size>& componentsToRemove)
{
	const auto first = connections[componentIdx][portIdx].otherComponent;
	connections[componentIdx][portIdx].setFree();

	LabwareSystem newSystem(*lab);
	std::queue<std::pair<l_size, l_size>> queue;

	// first component
	removeFromBoundary(components[first]->getBounds());
	newSystem.add(std::move(components[first]));
	newSystem.connections.emplace_back(std::move(connections[first]));
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

		newSystem.add(std::move(components[c.first]));
		newSystem.connections.emplace_back(std::move(connections[c.first]));
		componentsToRemove.emplace_back(c.first);

		for (uint8_t i = 0; i < newSystem.connections.back().size(); ++i)
			if (newSystem.connections.back()[i].isFree() == false &&
				newSystem.connections.back()[i].otherComponent != c.second)
			{
				queue.push(std::make_pair(newSystem.connections.back()[i].otherComponent, c.first));
				newSystem.connections.back()[i].otherComponent = checked_cast<l_size>(i + newSystem.components.size());
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
	//TODO: this couldl be written better
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

			components[componentIdx]->disconnect(lab->getAtmosphere(), *components[c.otherComponent]);
			components[c.otherComponent]->disconnect(lab->getAtmosphere(), *components[componentIdx]);

			const Amount<Unit::RADIAN> angle = components[c.otherComponent]->getPort(c.otherPort).angle +
				components[c.otherComponent]->getRotation() - 90.0_o;

			push.x += (components[componentIdx]->getPosition().x < components[c.otherComponent]->getPosition().x ?
				1.0f : -1.0f) * std::cos(angle.asStd());
			push.y += (components[componentIdx]->getPosition().y < components[c.otherComponent]->getPosition().y ?
				1.0f : -1.0f) * std::sin(angle.asStd());

			// one of the sub-sections can remain in this
			if (skippedFirst == false)
			{
				skippedFirst = true;
				continue;
			}
			result.emplace_back(releaseSection(componentIdx, i, toRemove));
			if (result.back().size() == 1)
				result.back().rotate(0.0f);
		}

		// components and connections are moved away but cannot be erased until the end because of indexes
		std::sort(toRemove.begin(), toRemove.end(), std::greater<>());
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
		for (auto& c : connections[componentIdx])
		{
			if (c.isFree())
				continue;

			components[componentIdx]->disconnect(lab->getAtmosphere(), *components[c.otherComponent]);
			components[c.otherComponent]->disconnect(lab->getAtmosphere(), *components[componentIdx]);

			const Amount<Unit::RADIAN> angle = components[c.otherComponent]->getPort(c.otherPort).angle +
				components[c.otherComponent]->getRotation() - 90.0_o;

			push.x += (components[componentIdx]->getPosition().x < components[c.otherComponent]->getPosition().x ?
				1.0f : -1.0f) * std::cos(angle.asStd());
			push.y += (components[componentIdx]->getPosition().y < components[c.otherComponent]->getPosition().y ?
				1.0f : -1.0f) * std::sin(angle.asStd());
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

	result.emplace_back(LabwareSystem(std::move(temp), *lab));
	return result;
}
