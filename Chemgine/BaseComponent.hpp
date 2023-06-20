#pragma once
#include "DataStore.hpp"
#include "ComponentType.hpp"

class BaseComponent
{
protected:
	const ComponentType type;
	static const DataStore* dataStore;

	BaseComponent(const ComponentType type);

public:

	virtual const AtomData& data() const = 0;

	static void setDataStore(const DataStore* const dataStore);
	static bool isCompositeType(const BaseComponent& component);
};