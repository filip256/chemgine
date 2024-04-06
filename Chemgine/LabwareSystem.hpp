#pragma once

#include "BaseLabwareComponent.hpp"
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
	BaseLabwareComponent& getComponent();
	const LabwareSystem& getSystem() const;
	LabwareSystem& getSystem();

	const DrawablePort* operator->() const;

	friend class LabwareSystem;
};



class Lab;

class LabwareSystem : public sf::Drawable
{
private:
	Ref<Lab> lab;
	std::vector<BaseLabwareComponent*> components;
	std::vector<std::vector<LabwareConnection>> connections;

	sf::FloatRect boundingBox = sf::FloatRect(
		std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	BaseLabwareComponent* releaseComponent(const l_size componentIdx);
	LabwareSystem releaseSection(
		const l_size componentIdx, const uint8_t portIdx,
		std::vector<l_size>& toRemove);

	void add(BaseLabwareComponent* component);
	static void add(PortIdentifier& otherPort, PortIdentifier& thisPort);

	void clearBoundry();
	void recomputeBoundry();
	void addToBoundry(const sf::FloatRect& box);
	void removeFromBoundry(const sf::FloatRect& box);
	/// <summary>
	/// Checks if the specified component is on the bounding box of the system.
	/// </summary>
	bool isOnBoundry(const sf::FloatRect& box) const;

	void recomputePositions(const l_size current, const uint8_t currentPort);

public:
	LabwareSystem(Lab& lab) noexcept;
	LabwareSystem(BaseLabwareComponent* component, Lab& lab) noexcept;
	LabwareSystem(const LabwareSystem&) = delete;
	LabwareSystem(LabwareSystem&&) = default;
	~LabwareSystem() noexcept;

	LabwareSystem& operator=(LabwareSystem&&) = default;

	l_size size() const;

	const BaseLabwareComponent& getComponent(const size_t idx) const;
	BaseLabwareComponent& getComponent(const size_t idx);

	void move(const sf::Vector2f& offset);
	void rotate(const float angle, const l_size center = 0);

	l_size contains(const sf::Vector2f& point) const;
	bool intersects(const LabwareSystem& other) const;

	bool isFree(const l_size componentIdx, const uint8_t portIdx) const;

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

	static bool isFree(const PortIdentifier& port);

	/// <summary>
	/// Connects 2 systems on the specified port.
	/// </summary>
	static void connect(PortIdentifier& destination, PortIdentifier& source);
	static bool canConnect(const PortIdentifier& destination, const PortIdentifier& source);
	
	/// <summary>
	/// Disconnects the specified component from this system and returns the new systems resulted
	/// from the split. The *this system is never fully emptied and it's not retuned.
	/// </summary>
	std::vector<LabwareSystem> disconnect(const l_size componentIdx);

	void tick(const Amount<Unit::SECOND> timespan);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

	constexpr static const l_size npos = static_cast<l_size>(-1);

	friend class PortIdentifier;
};
