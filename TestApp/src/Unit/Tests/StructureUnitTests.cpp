#include "Unit/Tests/StructureUnitTests.hpp"
#include "MolecularStructure.hpp"

#include <numeric>

StructureSMILESUnitTest::StructureSMILESUnitTest(
	const std::string& name,
	std::string&& smiles,
	const Amount<Unit::GRAM_PER_MOLE> expectedMass
) noexcept :
	UnitTest(name + '_' + smiles),
	expectedMass(expectedMass),
	smiles(std::move(smiles))
{}

bool StructureSMILESUnitTest::run()
{
	const auto parsedMolecule = MolecularStructure::create(smiles);
	if (not parsedMolecule)
	{
		Log(this).error("Failed to parse input SMILES: '{0}'.", smiles);
		return false;
	}

	const auto actualMass = parsedMolecule->getMolarMass();
	if (not Utils::floatEqual(actualMass.asStd(), expectedMass.asStd(), 0.005f))
	{
		Log(this).error("Parsed molecule's molar mass: {0} does not match the expected molar mass: {1}.", actualMass.toString(), expectedMass.toString());
		return false;
	}

	const auto printedSmiles = parsedMolecule->toSMILES();

	const auto printedMolecule = MolecularStructure::create(printedSmiles);
	if (not printedMolecule)
	{
		Log(this).error("Failed to parse printed SMILES: '{0}'.", printedSmiles);
		return false;
	}

	if (*parsedMolecule != *printedMolecule)
	{
		Log(this).error("Equality check between printed molecule: '{0}' and input failed.", printedSmiles);
		return false;
	}

	return true;
}


StructureEqualityUnitTest::StructureEqualityUnitTest(
	const std::string& name,
	const std::string& targetSmiles,
	const std::string& patternSmiles,
	const bool expected
) noexcept :
	UnitTest(name + '_' + targetSmiles + '_' + patternSmiles),
	expected(expected),
	target(targetSmiles),
	pattern(patternSmiles)
{}

bool StructureEqualityUnitTest::run()
{
	return (target == pattern) == expected;
}


StructureAtomMapUnitTest::StructureAtomMapUnitTest(
	const std::string& name,
	const std::string& targetSmiles,
	const std::string& patternSmiles,
	const bool expected
) noexcept :
	UnitTest(name + '_' + targetSmiles + '_' + patternSmiles),
	expected(expected),
	target(targetSmiles),
	pattern(patternSmiles)
{}

bool StructureAtomMapUnitTest::run()
{
	return (target.mapTo(pattern, true).size() > 0) == expected;
}


StructureMaximalAtomMapUnitTest::StructureMaximalAtomMapUnitTest(
	const std::string& name,
	const std::string& targetSmiles,
	const std::string& patternSmiles,
	const size_t expectedSize
) noexcept :
	UnitTest(name + '_' + targetSmiles + '_' + patternSmiles),
	expectedSize(expectedSize),
	target(targetSmiles),
	pattern(patternSmiles)
{}

bool StructureMaximalAtomMapUnitTest::run()
{
	const auto size = target.maximalMapTo(pattern).first.size();
	if (size != expectedSize)
	{
		Log(this).error("Actual map size: {0} is different from the expected size: {1}.", size, expectedSize);
		return false;
	}
	return true;
}


StructureSubstitutionUnitTest::StructureSubstitutionUnitTest(
	const std::string& name,
	const std::string& patternSmiles,
	const std::string& instanceSmiles,
	const std::string& expectedSmiles
) noexcept :
	UnitTest(name + '_' + patternSmiles + '_' + instanceSmiles),
	pattern(patternSmiles),
	instance(instanceSmiles),
	expected(expectedSmiles)
{}

bool StructureSubstitutionUnitTest::run()
{
	const auto map = instance.maximalMapTo(pattern).first;
	if (map.empty())
	{
		Log(this).error("Generating the atom mapping between instance: '{0}' and pattern: '{1}' failed.", instance.toSMILES(), pattern.toSMILES());
		return false;
	}

	const auto result = MolecularStructure::addSubstituents(pattern, instance, map);
	if (result != expected)
	{
		Log(this).error("Equality check between result: '{0}' and expected result: '{1}' failed.", result.toSMILES(), expected.toSMILES());
		return false;
	}

	return true;
}


FundamentalCycleUnitTest::FundamentalCycleUnitTest(
	const std::string& name,
	const std::string& moleculeSmiles,
	const size_t expectedCycleCount,
	const size_t expectedTotalCyclicAtomCount
) noexcept :
	UnitTest(name + '_' + moleculeSmiles),
	molecule(moleculeSmiles),
	expectedCycleCount(expectedCycleCount),
	expectedTotalCyclicAtomCount(expectedTotalCyclicAtomCount)
{}

bool FundamentalCycleUnitTest::run()
{
	const auto cycles = molecule.getFundamentalCycleBasis();
	if (cycles.size() != expectedCycleCount)
	{
		Log(this).error("Actual cycle count: {0} is different from the expected count: {1}.",
			cycles.size(), expectedCycleCount);
		return false;
	}

	// Counting the total amount of distinct atoms participating in cycles should
	// be agnostic to canonicalization and the starting node of the algorithm.
	std::unordered_set<c_size> atomSet;
	for (const auto& cycle : cycles)
		for (const auto atom : cycle)
			atomSet.emplace(atom);

	if (atomSet.size() != expectedTotalCyclicAtomCount)
	{
		Log(this).error("Actual total cyclic atom count: {0} is different from the expected atom count: {1}.",
			atomSet.size(), expectedTotalCyclicAtomCount);
		return false;
	}

	return true;
}


StructureUnitTests::StructureUnitTests(
	std::string&& name,
	const std::regex& filter,
	const std::string& atomsFilePath
) noexcept :
	UnitTestGroup(std::move(name), filter)
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide();
	dataStore.load(atomsFilePath);
	LogBase::unhide();

	registerTest<UnitTestSetup<AccessorTestSetup>>("setup", dataStore);

	registerTest<StructureSMILESUnitTest>("SMILES", "CN(C)C(=O)C1=CC=CC=C1", 149.19);
	registerTest<StructureSMILESUnitTest>("SMILES", "C1=CC=CC=C1R", 77.11);
	registerTest<StructureSMILESUnitTest>("SMILES", "CC(=O)OC", 74.08);
	registerTest<StructureSMILESUnitTest>("SMILES", "CC(=O)N(C)C", 87.12);
	registerTest<StructureSMILESUnitTest>("SMILES", "C1CC1", 42.08);
	registerTest<StructureSMILESUnitTest>("SMILES", "CC1CCCC2CCCCC12", 152.28);
	registerTest<StructureSMILESUnitTest>("SMILES", "RC1=CC2=C(NC=C2)C=C1", 116.15);
	registerTest<StructureSMILESUnitTest>("SMILES", "NC1C2=C1CN2", 82.11);
	registerTest<StructureSMILESUnitTest>("SMILES", "C1C2C1C2", 54.09);
	registerTest<StructureSMILESUnitTest>("SMILES", "CC1C2C1C2", 68.12);
	registerTest<StructureSMILESUnitTest>("SMILES", "CC1C2C1N2", 69.11);
	registerTest<StructureSMILESUnitTest>("SMILES", "OC1C2CC12", 70.09);
	registerTest<StructureSMILESUnitTest>("SMILES", "CNC1CC=C1C=C=CC3=C=CN3", 172.23);
	registerTest<StructureSMILESUnitTest>("SMILES", "CN4CCC15C=CC(O)C3OC2=C(O)C=CC(=CC1)C2C345", 285.34);
	registerTest<StructureSMILESUnitTest>("SMILES", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", 352.46);
	registerTest<StructureSMILESUnitTest>("SMILES", "CN1CC(C=C2C1CC3=CNC4=CC=CC2=C34)C(=O)O", 268.32);
	registerTest<StructureSMILESUnitTest>("SMILES", "[Mg](O)O", 58.32);
	registerTest<StructureSMILESUnitTest>("SMILES", "S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N", 270.31);
	registerTest<StructureSMILESUnitTest>("SMILES", "CC2CCCC(C1CCCCC1)C2", 180.33);
	registerTest<StructureSMILESUnitTest>("SMILES", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", 254.42);
	registerTest<StructureSMILESUnitTest>("SMILES", "C1C2C13C24C38C4%10C79C56CC5C67C89%10", 150.18);
	registerTest<StructureSMILESUnitTest>("SMILES", "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12", 356.47);
	registerTest<StructureSMILESUnitTest>("SMILES", "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC", 282.56);
	registerTest<StructureSMILESUnitTest>("SMILES", "HC(H)(H)C(H)(H)C(H)(H)H", 44.1);
	registerTest<StructureSMILESUnitTest>("SMILES", "C%12C3CCC%123C", 82.15);

	registerTest<StructureEqualityUnitTest>("equality", "CN(C)C(=O)C1=CC=CC=C1", "C1=CC=CC=C1R", false);
	registerTest<StructureEqualityUnitTest>("equality", "N1(C2(C1C(C)2))", "N1(C2(C1C2(C)))", true);
	registerTest<StructureEqualityUnitTest>("equality", "CC(=O)OC", "RC(=O)OR", false);
	registerTest<StructureEqualityUnitTest>("equality", "CC(=O)OC", "RC(=O)O", false);
	registerTest<StructureEqualityUnitTest>("equality", "CC(=O)N(C)C", "RC(=O)N(R)R", false);
	registerTest<StructureEqualityUnitTest>("equality", "O(C)(CC)", "O(C)(C)", false);
	registerTest<StructureEqualityUnitTest>("equality", "C1CC1", "C1CC1", true);
	registerTest<StructureEqualityUnitTest>("equality", "C1C(OC)CC1", "C1CC(OR)C1", false);
	registerTest<StructureEqualityUnitTest>("equality", "CC(O)C", "OR", false);
	registerTest<StructureEqualityUnitTest>("equality", "CC1CCCC2CCCCC12", "CC1CCCC2CCCCC12", true);
	registerTest<StructureEqualityUnitTest>("equality", "CC1=CC2=C(NC=C2)C=C1", "RC1=CC2=C(NC=C2)C=C1", false);
	registerTest<StructureEqualityUnitTest>("equality", "C1CC2=C1C=C2", "RC1=C(R)CC1", false);
	registerTest<StructureEqualityUnitTest>("equality", "C(C)(C)OC", "O(R)R", false);
	registerTest<StructureEqualityUnitTest>("equality", "O(CCC)CC", "O(CC)(CCC)", true);
	registerTest<StructureEqualityUnitTest>("equality", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R", false);
	registerTest<StructureEqualityUnitTest>("equality", "COC4=C2C1=C([N]C=C1CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C4", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R", false);
	registerTest<StructureEqualityUnitTest>("equality", "[Cl]", "X", false);

	// TODO: should check the size of the mapping
	// TODO: add reaction concretization tests
	// TODO: unit tests should show failed checks at the end
	registerTest<StructureAtomMapUnitTest>("map", "CN(C)C(=O)C1=CC=CC=C1", "C1=CC=CC=C1R", true);
	registerTest<StructureAtomMapUnitTest>("map", "CC(=O)OC", "RC(=O)OR", true);
	registerTest<StructureAtomMapUnitTest>("map", "CC(=O)OC", "RC(=O)O", false);
	registerTest<StructureAtomMapUnitTest>("map", "CC(=O)N(C)C", "RC(=O)N(R)R", true);
	registerTest<StructureAtomMapUnitTest>("map", "O(C)(CC)", "O(C)(C)", false);
	registerTest<StructureAtomMapUnitTest>("map", "C1CC1", "C1CC1", true);
	registerTest<StructureAtomMapUnitTest>("map", "C1C(OC)CC1", "C1CC(OR)C1", true);
	registerTest<StructureAtomMapUnitTest>("map", "CC(O)C", "OR", true);
	registerTest<StructureAtomMapUnitTest>("map", "CC1CCCC2CCCCC12", "CC1CCCC2CCCCC12", true);
	registerTest<StructureAtomMapUnitTest>("map", "CC1=CC2=C(NC=C2)C=C1", "RC1=CC2=C(NC=C2)C=C1", true);
	registerTest<StructureAtomMapUnitTest>("map", "C1CC2=C1C=C2", "RC1=C(R)CC1", true);
	registerTest<StructureAtomMapUnitTest>("map", "C(C)(C)OC", "O(R)R", true);
	registerTest<StructureAtomMapUnitTest>("map", "O(CCC)CC", "O(CC)(CCC)", true);
	registerTest<StructureAtomMapUnitTest>("map", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R", true);
	registerTest<StructureAtomMapUnitTest>("map", "COC4=C2C1=C([N]C=C1CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C4", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R", true);
	registerTest<StructureAtomMapUnitTest>("map", "[Cl]", "X", true);

	registerTest<StructureMaximalAtomMapUnitTest>("maximal_map", "CC(=O)OC", "OCC", 3);
	registerTest<StructureMaximalAtomMapUnitTest>("maximal_map", "C1CCCCC(O)CC1", "CC(O)C", 4);
	registerTest<StructureMaximalAtomMapUnitTest>("maximal_map", "CC(=O)OR", "OCR", 2);
	registerTest<StructureMaximalAtomMapUnitTest>("maximal_map", "C(=O)N(C)C", "C1CCC1", 1);
	registerTest<StructureMaximalAtomMapUnitTest>("maximal_map", "O(C)CC", "O(CC)C", 4);
	registerTest<StructureMaximalAtomMapUnitTest>("maximal_map", "CC2CCCC(C1CCCCC1)C2", "CC1CCCCC1", 7);

	registerTest<StructureSubstitutionUnitTest>("substitute", "CC(=O)OC", "OCCC", "O=C(OC)CC");
	registerTest<StructureSubstitutionUnitTest>("substitute", "CC(=O)OR", "OCCC", "O=C(OR)CC");
	registerTest<StructureSubstitutionUnitTest>("substitute", "CCC(=O)O", "CC(=O)OCC", "O=C(OCC)CC");
	registerTest<StructureSubstitutionUnitTest>("substitute", "C(=O)O", "CC(=O)OC(C)C", "CC(=O)OC(C)C");
	registerTest<StructureSubstitutionUnitTest>("substitute", "C1CCCC1", "C1CC(O)C1", "OC1CCCC1");
	registerTest<StructureSubstitutionUnitTest>("substitute", "CC(C)C", "C1CCC1O", "OC1(CCC1)(C)");
	registerTest<StructureSubstitutionUnitTest>("substitute", "C(=O)O", "CC(=O)OCCCCCCCCCCC", "O=C(OCCCCCCCCCCC)C");
	registerTest<StructureSubstitutionUnitTest>("substitute", "C1C2C(CC(C=O)CC2CCCC)CCC1CC(=O)OC", "CC(=O)OC(CCC2C1C(C(CC(CC=C)CC)CC2)C=O)C1", "O=CC2CC1C(CC(OC(=O)C)(CC1)CC(=O)OC)C(C2)C(CCC)CC=C");

	// TODO: This will result in a valence violation. Some check in substitute would be nice
	//registerTest<MolecularSubstitutionTest>("substitute", "CC(=C)C", "C1CCC1O", "OC1(CCC1)(=C)");

	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CC(=O)OC(C)C", 0, 0);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "OC1CCC1", 1, 4);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C1CCC(C)CC1", 1, 6);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CCC1CCC(O)CC1", 1, 6);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CC2CC(CCCC)CC(C1CCCCC1)C2", 2, 12);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", 4, 16);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", 7, 19);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C1CC12CC2", 2, 5);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12", 12, 28);
	registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C1C2CC3CC23C1", 3, 7);

	registerTest<UnitTestSetup<AccessorTestCleanup>>("cleanup");
	Accessor<>::unsetDataStore();
}
