#pragma once
#include "DataStore.hpp"

class DataStoreAccessor
{
private:
	const DataStore* dataStore = nullptr;

public:

	DataStoreAccessor() = default;

	void set(const DataStore& dataStore);
	void crashIfUninitialized() const;

	const DataStore& get() const;
	const DataStore& getSafe() const;
};