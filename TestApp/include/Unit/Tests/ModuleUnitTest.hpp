#pragma once

#include "Unit/UnitTest.hpp"
#include "DataStore.hpp"

class WaterPropertiesUnitTest : public UnitTestGroup
{
private:
	const Molecule molecule;

public:
	WaterPropertiesUnitTest(
		std::string&& name,
		const std::regex& filter
	) noexcept;
};


class ModuleUnitTest : public UnitTestGroup
{
private:
	DataStore dataStore;

public:
	ModuleUnitTest(
		const std::string& name,
		const std::regex& filter,
		const std::string& defModulePath
	) noexcept;
};
