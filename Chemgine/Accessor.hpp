#pragma once

#include "DataStoreAccessor.hpp"

template<typename DataT>
class Accessor
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	Accessor() = default;
	virtual ~Accessor() = default;

	virtual const DataT& data() const = 0;

	/// <summary>
	/// Similar to the static getDataStore but assumes that the null check has been done by the constructor
	/// </summary>
	const DataStore& dataStore() const;

public:
	/// <summary>
	/// Links the accessor to a DataStore.
	/// </summary>
	static void setDataStore(const DataStore& dataStore);

	/// <summary>
	/// Also checks that the data accessor was set, should be called by other static methods.
	/// </summary>
	static const DataStore& getDataStore();
};


template<typename DataT>
DataStoreAccessor Accessor<DataT>::dataAccessor = DataStoreAccessor();

template<typename DataT>
void Accessor<DataT>::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

template<typename DataT>
const DataStore& Accessor<DataT>::getDataStore()
{
	return dataAccessor.getSafe();
}

template<typename DataT>
const DataStore& Accessor<DataT>::dataStore() const
{
	return dataAccessor.get();
}
