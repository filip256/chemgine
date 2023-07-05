#pragma once

#include "ComponentType.hpp"
#include "BaseComponentData.hpp"
#include "DataStoreAccessor.hpp"

class DataStore;

class BaseComponent
{
private:
	static DataStoreAccessor dataAccessor;
protected:
	mutable ComponentIdType id;
	const ComponentType type;

	BaseComponent(const ComponentIdType id, const ComponentType type) noexcept;
	BaseComponent(BaseComponent&&) noexcept = default;

	/// <summary>
	/// Similar to the static getDataStore but assumes that the null check has been done by the constructor
	/// </summary>
	/// <returns></returns>
	const DataStore& dataStore() const;

public:
	~BaseComponent() noexcept = default;

	static size_t instanceCount;

	virtual const BaseComponentData& data() const = 0;

	ComponentIdType getId() const;

	bool isAtomicType() const;
	bool isCompositeType() const;

	static void setDataStore(const DataStore& dataStore);
	static const DataStore& getDataStore();

	static bool isCompositeType(const BaseComponent& component);


	// for memory leak checking 
	void* operator new(const size_t count);
	void operator delete(void* ptr);
};