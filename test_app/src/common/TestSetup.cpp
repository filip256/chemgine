#include "common/TestSetup.hpp"

#include "data/Accessor.hpp"
#include "data/DataStore.hpp"
#include "utils/Path.hpp"

AccessorTestSetup::AccessorTestSetup(const DataStore& dataStore) noexcept :
    dataStore(dataStore)
{}

void AccessorTestSetup::run() { Accessor<>::setDataStore(dataStore); }

void AccessorTestCleanup::run() { Accessor<>::unsetDataStore(); }

CreateDirTestSetup::CreateDirTestSetup(std::string&& directory) noexcept :
    directory(std::move(directory))
{}

void CreateDirTestSetup::run() { utils::createDir(directory); }

RemoveDirTestSetup::RemoveDirTestSetup(std::string&& directory) noexcept :
    directory(std::move(directory))
{}

void RemoveDirTestSetup::run() { utils::removeDir(directory); }
