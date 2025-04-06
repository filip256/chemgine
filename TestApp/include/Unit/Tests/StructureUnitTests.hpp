#pragma once

#include "Unit/UnitTest.hpp"
#include "DataStore.hpp"
#include "MolecularStructure.hpp"

class StructureSMILESUnitTest : public UnitTest
{
private:
	const Amount<Unit::GRAM_PER_MOLE> expectedMass;
	const std::string smiles;

public:
	StructureSMILESUnitTest(
		const std::string& name,
		std::string&& smiles,
		const Amount<Unit::GRAM_PER_MOLE> expectedMass
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


class FundamentalCycleUnitTest : public UnitTest
{
private:
	const c_size expectedCycleCount;
	const c_size expectedTotalCyclicAtomCount;
	const MolecularStructure molecule;

public:
	FundamentalCycleUnitTest(
		const std::string& name,
		const std::string& moleculeSmiles,
		const c_size expectedCycleCount,
		const c_size expectedTotalCyclicAtomCount
	) noexcept;

	bool run() override final;
};


class MinimalCycleUnitTest : public UnitTest
{
private:
	const c_size expectedTotalCyclicAtomCount;
	const std::unordered_map<c_size, c_size> expectedCycleSizes;
	const MolecularStructure molecule;

public:
	MinimalCycleUnitTest(
		const std::string& name,
		const std::string& moleculeSmiles,
		const c_size expectedTotalCyclicAtomCount,
	    std::unordered_map<c_size, c_size>&& expectedCycleSizes
	) noexcept;

	bool run() override final;
};


class ASCIIPrintUnitTest : public UnitTest
{
private:
	const bool allowLinearCycleExpansion;
	const MolecularStructure molecule;

public:
	ASCIIPrintUnitTest(
		const std::string& name,
		const std::string& moleculeSmiles,
		const bool allowLinearCycleExpansion
	) noexcept;

	bool run() override final;
};


class MolBinUnitTest : public UnitTest
{
private:
	const MolecularStructure molecule;

public:
	MolBinUnitTest(
		const std::string& name,
		const std::string& moleculeSmiles
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
