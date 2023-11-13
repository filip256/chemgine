#pragma once

#include "BaseLabwareComponent.hpp"
#include "PVector.hpp"
#include "SizeTypedefs.hpp"
#include "LabwareConnection.hpp"
#include "SFML/Graphics.hpp"

#include <limits>

class LabwareSystem;

class PortIdentifier
{
private:
	l_size componentIdx;
	uint8_t portIdx;
	LabwareSystem& system;

	PortIdentifier(
		LabwareSystem& system,
		const l_size componentIdx,
		const uint8_t portIdx
	) noexcept;

public:
	bool isValid() const;
	l_size getComponentIndex() const;
	uint8_t getPortIndex() const;
	const BaseLabwareComponent& getComponent() const;
	LabwareSystem& getSystem();

	const LabwarePort* operator->() const;

	friend class LabwareSystem;
};

class LabwareSystem
{
private:
	PVector<BaseLabwareComponent, l_size> components;
	std::vector<std::vector<LabwareConnection>> connections;

	sf::FloatRect boundingBox = sf::FloatRect(0.0f, 0.0f, -1.0f, -1.0f);

	BaseLabwareComponent* release(const l_size componentIdx);
	void add(BaseLabwareComponent* component);
	void add(BaseLabwareComponent* component, uint8_t componentPort, const PortIdentifier& thisPort);

public:
	LabwareSystem() = default;
	LabwareSystem(const BaseLabwareComponent& component) noexcept;
	LabwareSystem(const LabwareSystem&) = delete;
	LabwareSystem(LabwareSystem&&) = default;
	LabwareSystem& operator=(LabwareSystem&&) = default;

	l_size size() const;

	void add(const BaseLabwareComponent& component);

	void move(const sf::Vector2f& offset);

	l_size contains(const sf::Vector2f& point) const;
	bool intersects(const LabwareSystem& other) const;

	l_size findFirst() const;

	/// <summary>
	/// Finds the closest port to a given point and returns a pair of the port and the squared distance
	/// between itself and the point.
	/// Complexity: O(n)
	/// </summary>
	std::pair<PortIdentifier, float> findClosestPort(
		const sf::Vector2f& point,
		const float maxSqDistance = std::numeric_limits<float>::max()
	);

	/// <summary>
	/// Finds the closest ports from this and other and returns a pair of the two ports if the squared distance
	/// between them is less or equal to maxSqDistance.
	/// Complexity: O(n*m)
	/// </summary>
	std::pair<PortIdentifier, PortIdentifier> findClosestPort(
		LabwareSystem& other,
		const float maxSqDistance = std::numeric_limits<float>::max()
	);

	static void connect(PortIdentifier& destination, PortIdentifier& source);

	void draw(sf::RenderTarget& target) const;

	constexpr static const l_size npos = PVector<BaseLabwareComponent, l_size>::npos;

	friend class PortIdentifier;
};