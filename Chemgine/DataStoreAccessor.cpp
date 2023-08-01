#include "DataStoreAccessor.hpp"
#include "Logger.hpp"

void DataStoreAccessor::set(const DataStore& dataStore)
{
	if (this->dataStore)
		Logger::log("Already initilized data store accessor has been modified.", LogType::WARN);
	this->dataStore = &dataStore;
}

void DataStoreAccessor::crashIfUninitialized() const
{
	if (dataStore == nullptr)
		Logger::fatal("Tried to access a data store with an uninitialized accessor.");
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