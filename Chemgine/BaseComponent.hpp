#pragma once

#include <unordered_map>

#include "ComponentType.hpp"
#include "BaseComponentData.hpp"
#include "DataStoreAccessor.hpp"
#include "SizeTypedefs.hpp"

class BaseComponent
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	mutable ComponentIdType id;
	const ComponentType type;

	BaseComponent(const ComponentIdType id, const ComponentType type) noexcept;

	/// <summary>
	/// Similar to the static getDataStore but assumes that the null check has been done by the constructor
	/// </summary>
	const DataStore& dataStore() const;

public:
	~BaseComponent() noexcept = default;


	virtual const BaseComponentData& data() const = 0;

	ComponentIdType getId() const;

	bool isAtomicType() const;
	bool isCompositeType() const;

	static void setDataStore(const DataStore& dataStore);

	/// <summary>
	/// Also checks that the data accessor was set, should be called by other static methods.
	/// </summary>
	static const DataStore& getDataStore();

	static bool isCompositeType(const BaseComponent& component);

	/// <summary>
	/// Must be the exact atoms.
	/// </summary>
	static bool areEqual(const BaseComponent& x, const BaseComponent& y);
	/// <summary>
	/// Must be the exact atoms or satisfy radical matching.
	/// </summary>
	static bool areMatching(const BaseComponent& x, const BaseComponent& y);

	virtual bool isRadicalType() const;

	virtual uint8_t getPrecedence() const = 0;

	virtual std::unordered_map<ComponentIdType, c_size> getComponentCountMap() const = 0;

	virtual BaseComponent* clone() const = 0;

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};