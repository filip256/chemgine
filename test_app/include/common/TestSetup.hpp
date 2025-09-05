#pragma once

#include <string>

class DataStore;

class TestSetup
{
public:
    TestSetup()                 = default;
    TestSetup(const TestSetup&) = default;
    TestSetup(TestSetup&&)      = default;
    virtual ~TestSetup()        = default;

    virtual void run() = 0;
};

// DataStore's must be available twice, during both test initialization and execution.
// This setup loads and sets the DataStore once during initialization and sets it again during execution.
class AccessorTestSetup : public TestSetup
{
private:
    DataStore& dataStore;

public:
    AccessorTestSetup(DataStore& dataStore) noexcept;
    AccessorTestSetup(DataStore& dataStore, const std::string& loadPath) noexcept;

    void run() override final;
};

// This step clears the effects of AccessorTestSetup, ensuring a clean global context for future tests.
class AccessorTestCleanup : public TestSetup
{
public:
    AccessorTestCleanup() noexcept;

    void run() override final;
};

class CreateDirTestSetup : public TestSetup
{
private:
    const std::string directory;

public:
    CreateDirTestSetup(std::string&& directory) noexcept;

    void run() override final;
};

class RemoveDirTestSetup : public TestSetup
{
private:
    const std::string directory;

public:
    RemoveDirTestSetup(std::string&& directory) noexcept;

    void run() override final;
};
