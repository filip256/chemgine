#pragma once

#include "DataStoreAccessor.hpp"

/// <summary>
/// Implements data store related methods.
/// Different types may be provided as template arguments, allowing multiple data stores
/// to be used by certain derived types.
/// </summary>
template<typename UniqueT = void>
class Accessor
{
protected:
	static DataStoreAccessor dataAccessor;

	Accessor() = default;
	virtual ~Accessor() = default;

public:
	/// <summary>
	/// Similar to the static getDataStore but assumes that the null check has been done by the constructor
	/// </summary>
	static const DataStore& getDataStore();

	/// <summary>
	/// Links the accessor to a DataStore.
	/// </summary>
	static void setDataStore(const DataStore& dataStore);

	/// <summary>
	/// Unlinks the current DataStore of this accessor.
	/// </summary>
	static void unsetDataStore();
};

template<typename UniqueT>
DataStoreAccessor Accessor<UniqueT>::dataAccessor = DataStoreAccessor();

template<typename UniqueT>
const DataStore& Accessor<UniqueT>::getDataStore()
{
	return dataAccessor.get();
}

template<typename UniqueT>
void Accessor<UniqueT>::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

template<typename UniqueT>
void Accessor<UniqueT>::unsetDataStore()
{
	dataAccessor.unset();
}
