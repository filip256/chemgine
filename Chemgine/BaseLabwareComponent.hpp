#pragma once

#include "BaseLabwareData.hpp"
#include "DataStoreAccessor.hpp"
#include "DrawablePort.hpp"
#include "SFML/Graphics.hpp"

class BaseLabwareComponent : public sf::Drawable
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	const BaseLabwareData& data;

	BaseLabwareComponent(const LabwareId id) noexcept;

	const DataStore& dataStore() const;

public:
	const LabwareId id;

	~BaseLabwareComponent() noexcept = default;

	static void setDataStore(const DataStore& dataStore);

	bool isContainerType() const;

	virtual const BaseLabwareData& getData() const = 0;
	virtual const sf::Sprite& getSprite() const = 0;
	virtual sf::Sprite& getSprite() = 0;
	virtual const sf::Vector2f& getPosition() const = 0;
	virtual const sf::Vector2f getAdjustedPosition() const = 0;
	virtual void setPosition(const sf::Vector2f& position) = 0;
	virtual void move(const sf::Vector2f& offset) = 0;
	virtual float getRotation() const = 0;
	virtual void setRotation(const float angle) = 0;
	virtual const sf::Vector2f& getOrigin() const = 0;
	virtual sf::FloatRect getBounds() const = 0;

	virtual const DrawablePort& getPort(const uint8_t idx) const = 0;
	virtual const std::vector<DrawablePort>& getPorts() const = 0;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual bool contains(const sf::Vector2f& point) const = 0;
	virtual bool intersects(const BaseLabwareComponent& other) const = 0;

	virtual BaseLabwareComponent* clone() const = 0;


	// for memory leak checking
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};
