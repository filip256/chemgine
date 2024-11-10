#pragma once

#include "PerfTest.hpp"

class DataStore;

class AccessorPerfTestSetup : public PerfTestSetup
{
private:
	const DataStore& dataStore;

public:
	AccessorPerfTestSetup(
		std::string&& name,
		const DataStore& dataStore
	) noexcept;

	void task() override final;
};


class AccessorPerfTestCleanup : public PerfTestSetup
{
public:
	using PerfTestSetup::PerfTestSetup;

	void task() override final;
};
