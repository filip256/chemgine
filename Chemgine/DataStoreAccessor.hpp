#pragma once

// Break cyclic dependency: BaseComponent <- DataStore <- MolecularStructure <- BaseComponent
class DataStore;

/// <summary>
/// Encapsulates a pointer to a DataStore object.
/// </summary>

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