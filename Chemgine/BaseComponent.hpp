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
	static size_t instanceCount;

	virtual const BaseComponentData& data() const = 0;

	static void setDataStore(const DataStore* const dataStore);
	static bool isCompositeType(const BaseComponent& component);


	// for memory leak checking 
	void* operator new(const size_t count);
	void operator delete(void* ptr);
};