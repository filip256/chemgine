#include "PerfTestSetup.hpp"
#include "DataStore.hpp"

AccessorPerfTestSetup::AccessorPerfTestSetup(
	std::string&& name,
	const DataStore& dataStore
) noexcept:
	PerfTestSetup(std::move(name)),
	dataStore(dataStore)
{}

void AccessorPerfTestSetup::task()
{
	Accessor<>::setDataStore(dataStore);
}


void AccessorPerfTestCleanup::task()
{
	Accessor<>::unsetDataStore();
}
