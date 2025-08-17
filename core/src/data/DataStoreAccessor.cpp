#include "data/DataStoreAccessor.hpp"

#include "io/Log.hpp"

const DataStore& DataStoreAccessor::get() const
{
	if (dataStore == nullptr)
		Log(this).fatal("Tried to access a data store with an uninitialized accessor.");
	return *dataStore;
}

void DataStoreAccessor::set(const DataStore& dataStore)
{
	if (this->dataStore != nullptr)
		Log(this).warn("Already initialized data store accessor has been modified.");
	this->dataStore = &dataStore;
}

void DataStoreAccessor::unset()
{
	dataStore = nullptr;
}
