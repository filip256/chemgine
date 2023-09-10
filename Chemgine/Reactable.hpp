#pragma once

#include "DataStoreAccessor.hpp"
#include "MolecularStructure.hpp"

class ReactableFactory;

class Reactable
{
private:
	static DataStoreAccessor dataAccessor;

protected:
	Reactable() noexcept;

	const DataStore& dataStore() const;

public:
	virtual const MolecularStructure& getStructure() const = 0;
	virtual std::unordered_map<c_size, c_size> matchWith(const MolecularStructure& structure) const = 0;

	static void setDataStore(const DataStore& dataStore);

	friend class ReactableFactory;

	// for memory leak checking 
	static size_t instanceCount;
#ifndef NDEBUG
	void* operator new(const size_t count);
	void operator delete(void* ptr);
#endif
};