#pragma once

class DataStore;

class DataStoreAccessor
{
private:
	const DataStore* dataStore = nullptr;

public:
	DataStoreAccessor() = default;

	const DataStore& get() const;

	void set(const DataStore& dataStore);
	void unset();
};
