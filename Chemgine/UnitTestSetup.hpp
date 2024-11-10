#pragma once

#include "UnitTest.hpp"
#include "Accessor.hpp"
#include "DataStore.hpp"

class AccessorUnitTestSetup : public UnitTestSetup
{
private:
	const DataStore& dataStore;

public:
	AccessorUnitTestSetup(
		std::string&& name,
		const DataStore& dataStore
	) noexcept;

	void task() override final;
};


class AccessorUnitTestCleanup : public UnitTestSetup
{
public:
	using UnitTestSetup::UnitTestSetup;

	void task() override final;
};
