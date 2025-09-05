#include "common/TestSetup.hpp"

#include "data/Accessor.hpp"
#include "data/DataStore.hpp"
#include "utils/Path.hpp"

//
// AccessorTestSetup
//

AccessorTestSetup::AccessorTestSetup(DataStore& dataStore) noexcept :
    dataStore(dataStore)
{}

AccessorTestSetup::AccessorTestSetup(DataStore& dataStore, const std::string& loadPath) noexcept :
    AccessorTestSetup(dataStore)
{
    Accessor<>::setDataStore(dataStore);

    LogBase::hide();
    dataStore.load(loadPath);
    LogBase::unhide();
}

void AccessorTestSetup::run() { Accessor<>::setDataStore(dataStore); }

//
// AccessorTestCleanup
//

AccessorTestCleanup::AccessorTestCleanup() noexcept { Accessor<>::unsetDataStore(); }

void AccessorTestCleanup::run() { Accessor<>::unsetDataStore(); }

CreateDirTestSetup::CreateDirTestSetup(std::string&& directory) noexcept :
    directory(std::move(directory))
{}

void CreateDirTestSetup::run() { utils::createDir(directory); }

RemoveDirTestSetup::RemoveDirTestSetup(std::string&& directory) noexcept :
    directory(std::move(directory))
{}

void RemoveDirTestSetup::run() { utils::removeDir(directory); }
