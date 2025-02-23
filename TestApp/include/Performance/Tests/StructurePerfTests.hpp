#pragma once

#include "Performance/PerfTest.hpp"
#include "DataStore.hpp"
#include "MolecularStructure.hpp"

class StructureSMILESPerfTest : public TimedTest
{
private:
	volatile bool dontOptimize = true;
	const std::string smiles;
	MolecularStructure molecule;

public:
	StructureSMILESPerfTest(
		const std::string& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		std::string&& smiles
	) noexcept;

	void task() override final;
	void postTask() override final;
};


class StructureOpsPerfTestBase : public TimedTest
{
protected:
	volatile bool dontOptimize = true;
	const MolecularStructure target;
	const MolecularStructure pattern;

public:
	StructureOpsPerfTestBase(
		const std::string& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		const std::string& targetSmiles,
		const std::string& patternSmiles
	) noexcept;
};


class StructureEqualityPerfTest : public StructureOpsPerfTestBase
{
public:
	using StructureOpsPerfTestBase::StructureOpsPerfTestBase;

	void task() override final;
};


class StructureInequalityPerfTest : public StructureOpsPerfTestBase
{
public:
	using StructureOpsPerfTestBase::StructureOpsPerfTestBase;

	void task() override final;
};


class StructureAtomMapPerfTest : public StructureOpsPerfTestBase
{
public:
	using StructureOpsPerfTestBase::StructureOpsPerfTestBase;

	void task() override final;
};


class StructureMaximalAtomMapPerfTest : public StructureOpsPerfTestBase
{
public:
	using StructureOpsPerfTestBase::StructureOpsPerfTestBase;

	void task() override final;
};


class StructureSubstitutionPerfTest : public StructureOpsPerfTestBase
{
private:
	std::unordered_map<c_size, c_size> atomMap;

public:
	StructureSubstitutionPerfTest(
		const std::string& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		const std::string& patternSmiles,
		const std::string& intanceSmiles
	) noexcept;

	void task() override final;
};


class StructurePerfTests : public PerfTestGroup
{
private:
	DataStore dataStore;

public:
	StructurePerfTests(
		std::string&& name,
		const std::regex& filter,
		const std::string& atomsFilePath
	) noexcept;
};
