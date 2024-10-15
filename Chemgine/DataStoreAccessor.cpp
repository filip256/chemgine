#include "DataStoreAccessor.hpp"
#include "Log.hpp"

void DataStoreAccessor::set(const DataStore& dataStore)
{
	if (this->dataStore)
		Log(this).warn("Already initilized data store accessor has been modified.");
	this->dataStore = &dataStore;
}

const DataStore& DataStoreAccessor::get() const
{
	if (dataStore == nullptr)
		Log(this).fatal("Tried to access a data store with an uninitialized accessor.");
	return *dataStore;
}
