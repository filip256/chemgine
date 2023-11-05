#pragma once

#include "BaseLabwareComponent.hpp"
#include "PVector.hpp"
#include "SizeTypedefs.hpp"
#include "LabwareConnection.hpp"
#include "SFML/Graphics.hpp"

#include <limits>

typedef std::pair<l_size, uint8_t> PortIterator;

class LabwareSystem
{
private:
	PVector<BaseLabwareComponent, l_size> components;
	std::vector<std::vector<LabwareConnection>> connections;

	sf::FloatRect boundingBox = sf::FloatRect(0.0f, 0.0f, -1.0f, -1.0f);

public:
	LabwareSystem() = default;
	LabwareSystem(const BaseLabwareComponent& component) noexcept;
	LabwareSystem(const LabwareSystem&) = delete;
	LabwareSystem(LabwareSystem&&) = default;

	void add(const BaseLabwareComponent& component);

	void move(const sf::Vector2f& offset);

	l_size contains(const sf::Vector2f& point) const;
	bool intersects(const LabwareSystem& other) const;

	l_size findFirst() const;

	PortIterator findClosestPort(
		const sf::Vector2f& point,
		const float maxSqDistance = std::numeric_limits<float>::max()
	) const;

	void draw(sf::RenderTarget& target) const;

	constexpr static const l_size npos = PVector<BaseLabwareComponent, l_size>::npos;
};