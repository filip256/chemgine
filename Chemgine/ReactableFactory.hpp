#pragma once

#include "DataStoreAccessor.hpp"
#include "Reactable.hpp"

class ReactableFactory
{
public:
	/// <summary>
	/// Sets the data accessor for Reactables
	/// </summary>
	/// <param name="dataStore"></param>
	static void setDataStore(const DataStore& dataStore);

	/// <summary>
	/// Allocates the right type of Reactable and returns a pointer to it or nullptr if the operation fails.
	/// ReactableFactory does not own Reactables created in this manner.
	/// </summary>
	/// <param name="id"></param>
	static const Reactable* get(const ComponentIdType id);
};