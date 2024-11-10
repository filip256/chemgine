#include "Lab.hpp"
#include "ColorCast.hpp"

Lab::Lab() noexcept :
	atmosphere(Atmosphere::createDefaultAtmosphere())
{}

Lab::Lab(Atmosphere&& atmosphere) noexcept :
	atmosphere(std::move(atmosphere))
{}

void Lab::add(LabwareSystem&& system)
{
	systems.emplace_back(std::move(system));
}

void Lab::removeEmptySystems()
{
	systems.erase(
		std::remove_if(systems.begin(), systems.end(), [](const LabwareSystem& s) { return s.size() == 0; }),
		systems.end());
}

size_t Lab::getSystemCount() const
{
	return systems.size();
}

const LabwareSystem& Lab::getSystem(const size_t idx) const
{
	return systems[idx];
}

LabwareSystem& Lab::getSystem(const size_t idx)
{
	return systems[idx];
}

const Atmosphere& Lab::getAtmosphere() const
{
	return atmosphere;
}

Atmosphere& Lab::getAtmosphere()
{
	return atmosphere;
}

size_t Lab::getSystemAt(const sf::Vector2f& point) const
{
	for (size_t i = 0; i < systems.size(); ++i)
		if (systems[i].contains(point) != LabwareSystem::npos)
			return i;
	return npos;
}

std::pair<size_t, l_size> Lab::getSystemComponentAt(const sf::Vector2f& point) const
{
	for (size_t i = 0; i < systems.size(); ++i)
		if (const auto c = systems[i].contains(point); c != LabwareSystem::npos)
			return std::make_pair(i, c);
	return std::make_pair(npos, LabwareSystem::npos);
}

size_t Lab::anyIntersects(const size_t targetIdx) const
{
	for (size_t i = 0; i < systems.size(); ++i)
		if (i != targetIdx && systems[i].intersects(systems[targetIdx]))
			return i;
	return npos;
}

bool Lab::tryConnect(const size_t targetIdx, const float_s maxSqDistance)
{
	for (size_t i = 0; i < systems.size(); ++i)
	{
		if (i == targetIdx)
			continue;

		auto ports = systems[i].findClosestPort(systems[targetIdx], maxSqDistance);
		if (ports.first.isValid() && LabwareSystem::canConnect(ports.first, ports.second))
		{
			LabwareSystem::connect(ports.first, ports.second);
			return true;
		}
	}
	return false;
}

bool Lab::tryDissconnect(const sf::Vector2f& point)
{
	for (size_t i = 0; i < systems.size(); ++i)
	{
		auto& sys = systems[i];
		if (sys.size() < 2)
			continue;

		const auto component = sys.contains(point);
		if (component == LabwareSystem::npos)
			continue;

		auto newSystems = sys.disconnect(component);
		for (size_t j = 0; j < newSystems.size(); ++j)
			add(std::move(newSystems[j]));

		return true;
	}
	return false;
}

Lab::LabSystemsConstIterator Lab::getSystemsBegin() const
{
	return systems.cbegin();
}

Lab::LabSystemsConstIterator Lab::getSystemsEnd() const
{
	return systems.cend();
}

Lab::LabSystemsIterator Lab::getSystemsBegin()
{
	return systems.begin();
}

Lab::LabSystemsIterator Lab::getSystemsEnd()
{
	return systems.end();
}

void Lab::tick(const Amount<Unit::SECOND> timespan)
{
	for (size_t i = 0; i < systems.size(); ++i)
		systems[i].tick(timespan);
	atmosphere.tick(timespan);
}

void Lab::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	const auto screenSize = target.getSize();
	atmosphereOverlay.setSize(sf::Vector2f(screenSize.x, screenSize.y));
	atmosphereOverlay.setFillColor(colorCast(atmosphere.getLayerColor()));
	target.draw(atmosphereOverlay, states);

	for (size_t i = 0; i < systems.size(); ++i)
		target.draw(systems[i], states);
}
