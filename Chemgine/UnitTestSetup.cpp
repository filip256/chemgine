#include "UnitTestSetup.hpp"

AccessorUnitTestSetup::AccessorUnitTestSetup(
	std::string&& name,
	const DataStore& dataStore
) noexcept :
	UnitTestSetup(std::move(name)),
	dataStore(dataStore)
{}

void AccessorUnitTestSetup::task()
{
	Accessor<>::setDataStore(dataStore);
}


void AccessorUnitTestCleanup::task()
{
	Accessor<>::unsetDataStore();
}
