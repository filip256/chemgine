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

class AccessorTestSetup : public TestSetup
{
private:
    const DataStore& dataStore;

public:
    AccessorTestSetup(const DataStore& dataStore) noexcept;

    void run() override final;
};

class AccessorTestCleanup : public TestSetup
{
public:
    using TestSetup::TestSetup;

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
