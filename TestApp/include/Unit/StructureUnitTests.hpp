#pragma once

#include "UnitTest.hpp"
#include "DataStore.hpp"
#include "MolecularStructure.hpp"

class StructureSMILESParseUnitTest : public UnitTest
{
private:
	const std::string smiles;

public:
	StructureSMILESParseUnitTest(
		const std::string& name,
		std::string&& smiles
	) noexcept;

	bool run() override final;
};


class StructureSMILESPrintUnitTest : public UnitTest
{
private:
	const MolecularStructure molecule;

public:
	StructureSMILESPrintUnitTest(
		const std::string& name,
		const std::string& smiles
	) noexcept;

	bool run() override final;
};


class StructureEqualityUnitTest : public UnitTest
{
private:
	const bool expected;
	const MolecularStructure target;
	const MolecularStructure pattern;

public:
	StructureEqualityUnitTest(
		const std::string& name,
		const std::string& targetSmiles,
		const std::string& patternSmiles,
		const bool expected
	) noexcept;

	bool run() override final;
};


class StructureAtomMapUnitTest : public UnitTest
{
private:
	const bool expected;
	const MolecularStructure target;
	const MolecularStructure pattern;

public:
	StructureAtomMapUnitTest(
		const std::string& name,
		const std::string& targetSmiles,
		const std::string& patternSmiles,
		const bool expected
	) noexcept;

	bool run() override final;
};


class StructureMaximalAtomMapUnitTest : public UnitTest
{
private:
	const size_t expectedSize;
	const MolecularStructure target;
	const MolecularStructure pattern;

public:
	StructureMaximalAtomMapUnitTest(
		const std::string& name,
		const std::string& targetSmiles,
		const std::string& patternSmiles,
		const size_t expectedSize
	) noexcept;

	bool run() override final;
};


class StructureSubstitutionUnitTest : public UnitTest
{
private:
	const MolecularStructure pattern;
	const MolecularStructure instance;
	const MolecularStructure expected;

public:
	StructureSubstitutionUnitTest(
		const std::string& name,
		const std::string& patternSmiles,
		const std::string& instanceSmiles,
		const std::string& expectedSmiles
	) noexcept;

	bool run() override final;
};


class StructureMassUnitTest : public UnitTest
{
private:
	const Amount<Unit::GRAM_PER_MOLE> expectedMass;
	const MolecularStructure molecule;

public:
	StructureMassUnitTest(
		const std::string& name,
		const std::string& smiles,
		const Amount<Unit::GRAM_PER_MOLE> expectedMass
	) noexcept;

	bool run() override final;
};


class StructureUnitTests : public UnitTestGroup
{
private:
	DataStore dataStore;

public:
	StructureUnitTests(
		std::string&& name,
		const std::regex& filter,
		const std::string& atomsFilePath
	) noexcept;
};
