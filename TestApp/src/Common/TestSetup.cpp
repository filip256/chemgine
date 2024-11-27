#include "Common/TestSetup.hpp"
#include "DataStore.hpp"
#include "PathUtils.hpp"
#include "Accessor.hpp"

AccessorTestSetup::AccessorTestSetup(
	const DataStore& dataStore
) noexcept :
	dataStore(dataStore)
{}

void AccessorTestSetup::run()
{
	Accessor<>::setDataStore(dataStore);
}


void AccessorTestCleanup::run()
{
	Accessor<>::unsetDataStore();
}


CreateDirTestSetup::CreateDirTestSetup(
	std::string&& directory
) noexcept :
	directory(std::move(directory))
{}

void CreateDirTestSetup::run()
{
	Utils::createDir(directory);
}


RemoveDirTestSetup::RemoveDirTestSetup(
	std::string&& directory
) noexcept :
	directory(std::move(directory))
{}

void RemoveDirTestSetup::run()
{
	Utils::removeDir(directory);
}
