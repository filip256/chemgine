#pragma once

#include "Unit/UnitTest.hpp"
#include "DataStore.hpp"

class DefUnitTest : public UnitTest
{
private:
	const std::string defLine;
	DataStore dataStore;

public:
	DefUnitTest(
		std::string&& name,
		std::string&& defLine
	) noexcept;

	bool run() override final;
};


class DefLoadUnitTest : public UnitTest
{
private:
	DataStore& dataStore;
	const std::string path;
	const bool expectedSuccess;

public:
	DefLoadUnitTest(
		std::string&& name,
		DataStore& dataStore,
		std::string&& path,
		const bool expectedSuccess
	) noexcept;

	bool run() override final;
};


class DefCountUnitTest : public UnitTest
{
private:
	const DataStore& dataStore;
	const size_t expectedDefCount;

public:
	DefCountUnitTest(
		std::string&& name,
		const DataStore& dataStore,
		const size_t expectedDefCount
	) noexcept;

	bool run() override final;
};


class DefDumpUnitTest : public UnitTest
{
private:
	const bool prettify;
	DataStore& dataStore;
	const std::string path;

public:
	DefDumpUnitTest(
		std::string&& name,
		DataStore& dataStore,
		std::string&& path,
		const bool prettify
	) noexcept;

	bool run() override final;
};


class DefClearUnitTest : public UnitTest
{
private:
	DataStore& dataStore;

public:
	DefClearUnitTest(
		std::string&& name,
		DataStore& dataStore
	) noexcept;

	bool run() override final;
};


class DefUnitTests : public UnitTestGroup
{
private:
	DataStore dataStore;

public:
	DefUnitTests(
		std::string&& name,
		const std::regex& filter,
		const std::string& baseDefFilePath
	) noexcept;
};
