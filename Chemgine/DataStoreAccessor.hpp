#pragma once

class DataStore;

class DataStoreAccessor
{
private:
	const DataStore* dataStore = nullptr;

public:
	DataStoreAccessor() = default;

	void set(const DataStore& dataStore);
	const DataStore& get() const;
};
