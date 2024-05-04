#pragma once

#include "BaseLabwareData.hpp"
#include "DataStoreAccessor.hpp"
#include "DrawableLabwareConnection.hpp"
#include "Molecule.hpp"
#include "Mixture.hpp"
#include "SFML/Graphics.hpp"

class PortIdentifier;

class BaseLabwareComponent : public sf::Drawable
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	const BaseLabwareData& data;

	BaseLabwareComponent(
		const LabwareId id,
		const LabwareType type
	) noexcept;

	const DataStore& dataStore() const;

public:
	const LabwareId id;

	virtual ~BaseLabwareComponent() = default;

	static void setDataStore(const DataStore& dataStore);

	bool isFlask() const;
	bool isAdaptor() const;
	bool isCondenser() const;
	bool isHeatsource() const;
	virtual bool isContainer() const;

	virtual const BaseLabwareData& getData() const = 0;
	virtual const sf::Sprite& getSprite() const = 0;
	virtual sf::Sprite& getSprite() = 0;
	virtual const sf::Vector2f& getPosition() const = 0;
	virtual const sf::Vector2f getAdjustedPosition() const = 0;
	virtual void move(const sf::Vector2f& offset) = 0;
	virtual Amount<Unit::DEGREE> getRotation() const = 0;
	virtual const sf::Vector2f& getOrigin() const = 0;
	virtual sf::FloatRect getBounds() const = 0;

	uint8_t getPortCount() const;
	virtual const DrawableLabwareConnection& getPort(const uint8_t idx) const = 0;
	virtual const std::vector<DrawableLabwareConnection>& getPorts() const = 0;

	virtual void setPosition(const sf::Vector2f& position) = 0;
	virtual void setRotation(const Amount<Unit::DEGREE> angle) = 0;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual bool contains(const sf::Vector2f& point) const = 0;
	virtual bool intersects(const BaseLabwareComponent& other) const = 0;

	virtual bool tryConnect(PortIdentifier& thisPort, PortIdentifier& otherPort);
	virtual void disconnect(PortIdentifier& thisPort, PortIdentifier& otherPortconst Ref<BaseContainer> dump);

	virtual void tick(const Amount<Unit::SECOND> timespan);

	template<typename L, typename = std::enable_if<std::is_base_of_v<BaseLabwareComponent, L>>>
	L& as();
	template<typename L, typename = std::enable_if<std::is_base_of_v<BaseLabwareComponent, L>>>
	Ref<L> cast();

	// for memory leak checking
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};

template<typename L, typename>
L& BaseLabwareComponent::as()
{
	return Ref(*this).as<L>();
}


template<typename L, typename>
Ref<L> BaseLabwareComponent::cast()
{
	return Ref(*this).cast<L>();
}
