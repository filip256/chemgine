#pragma once

#include "DataStoreAccessor.hpp"
#include "Reactable.hpp"

class ReactableFactory
{
public:
	ReactableFactory() noexcept;

	/// <summary>
	/// Allocates the right type of Reactable and returns a pointer to it or nullptr if the operation fails.
	/// ReactableFactory does not own Reactables created in this manner.
	/// </summary>
	const Reactable* get(const std::string& smiles) const;

	/// <summary>
	/// Allocates the right type of Reactable and returns a pointer to it or nullptr if the operation fails.
	/// ReactableFactory does not own Reactables created in this manner.
	/// </summary>
	const Reactable* get(const ComponentIdType id) const;

	/// <summary>
	/// Sets the data accessor for Reactables
	/// </summary>
	static void setDataStore(const DataStore& dataStore);

};