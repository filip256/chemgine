#include "DataStoreAccessor.hpp"
#include "Logger.hpp"

void DataStoreAccessor::set(const DataStore& dataStore)
{
	this->dataStore = &dataStore;
}

void DataStoreAccessor::crashIfUninitialized() const
{
	if (dataStore == nullptr)
		Logger::fatal("Tried to access an uninitialized data store.");
}

const DataStore& DataStoreAccessor::get() const
{
	return *dataStore;
}

const DataStore& DataStoreAccessor::getSafe() const
{
	crashIfUninitialized();
	return *dataStore;
}