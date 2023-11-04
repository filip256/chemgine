#pragma once

#include "BaseLabwareData.hpp"
#include "DataStoreAccessor.hpp"

class BaseLabwareComponent
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	const BaseLabwareData& const data;

	BaseLabwareComponent(const LabwareIdType id) noexcept;

public:
	~BaseLabwareComponent() noexcept = default;

	static void setDataStore(const DataStore& dataStore);

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};