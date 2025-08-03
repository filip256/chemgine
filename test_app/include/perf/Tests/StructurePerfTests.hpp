#pragma once

#include "perf/PerfTest.hpp"
#include "DataStore.hpp"
#include "MolecularStructure.hpp"

class StructureSMILESPerfTest : public TimedTest
{
private:
	volatile bool dontOptimize = true;
	const std::string smiles;

public:
	StructureSMILESPerfTest(
		const std::string& name,
		const std::variant<size_t, std::chrono::nanoseconds> limit,
		std::string&& smiles
	) noexcept;

	void task() override final;
};


class StructurePerfTestBase : public TimedTest
{
protected:
	volatile bool dontOptimize = true;
	const MolecularStructure target;

public:
	StructurePerfTestBase(
		const std::string& name,
		const std::variant<size_t, std::chrono::nanoseconds> limit,
		const std::string& targetSmiles
	) noexcept;
};


class StructureComparePerfTestBase : public TimedTest
{
protected:
	volatile bool dontOptimize = true;
	const MolecularStructure target;
	const MolecularStructure pattern;

public:
	StructureComparePerfTestBase(
		const std::string& name,
		const std::variant<size_t, std::chrono::nanoseconds> limit,
		const std::string& targetSmiles,
		const std::string& patternSmiles
	) noexcept;
};


class StructureEqualityPerfTest : public StructureComparePerfTestBase
{
public:
	using StructureComparePerfTestBase::StructureComparePerfTestBase;

	void task() override final;
};


class StructureInequalityPerfTest : public StructureComparePerfTestBase
{
public:
	using StructureComparePerfTestBase::StructureComparePerfTestBase;

	void task() override final;
};


class StructureAtomMapPerfTest : public StructureComparePerfTestBase
{
public:
	using StructureComparePerfTestBase::StructureComparePerfTestBase;

	void task() override final;
};


class StructureMaximalAtomMapPerfTest : public StructureComparePerfTestBase
{
public:
	using StructureComparePerfTestBase::StructureComparePerfTestBase;

	void task() override final;
};


class StructureSubstitutionPerfTest : public StructureComparePerfTestBase
{
private:
	const std::unordered_map<c_size, c_size> atomMap;

public:
	StructureSubstitutionPerfTest(
		const std::string& name,
		const std::variant<size_t, std::chrono::nanoseconds> limit,
		const std::string& patternSmiles,
		const std::string& intanceSmiles
	) noexcept;

	void task() override final;
};


class StructureFundamentalCyclePerfTest : public StructurePerfTestBase
{
public:
	using StructurePerfTestBase::StructurePerfTestBase;

	void task() override final;
};


class StructureMinimalCyclePerfTest : public StructurePerfTestBase
{
public:
	using StructurePerfTestBase::StructurePerfTestBase;

	void task() override final;
};


class ASCIIPrintTest : public StructurePerfTestBase
{
public:
	using StructurePerfTestBase::StructurePerfTestBase;

	void task() override final;
};


class ASCIIParseTest : public TimedTest
{
private:
	volatile bool dontOptimize = true;
	const std::string ascii;

	static std::string generateASCII(const std::string& smiles);

public:
	ASCIIParseTest(
		const std::string& name,
		const std::variant<size_t, std::chrono::nanoseconds> limit,
		const std::string& smiles
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
