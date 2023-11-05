#pragma once

#include "BaseLabwareData.hpp"
#include "DataStoreAccessor.hpp"
#include "SFML/Graphics.hpp"

class BaseLabwareComponent
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	const BaseLabwareData& data;

	BaseLabwareComponent(const LabwareIdType id) noexcept;

	const DataStore& dataStore() const;

public:
	~BaseLabwareComponent() noexcept = default;

	static void setDataStore(const DataStore& dataStore);

	bool isContainerType() const;

	virtual const BaseLabwareData& getData() const = 0;
	virtual const sf::Sprite& getSprite() const = 0;
	virtual sf::Sprite& getSprite() = 0;
	virtual void draw(sf::RenderTarget& target) const = 0;
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