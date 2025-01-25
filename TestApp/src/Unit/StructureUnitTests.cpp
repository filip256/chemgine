#include "Unit/StructureUnitTests.hpp"
#include "MolecularStructure.hpp"

StructureSMILESParseUnitTest::StructureSMILESParseUnitTest(
	const std::string& name,
	std::string&& smiles
) noexcept :
	UnitTest(name + '_' + smiles),
	smiles(std::move(smiles))
{}

bool StructureSMILESParseUnitTest::run()
{
	const auto molecule = MolecularStructure::create(smiles);
	if (not molecule)
	{
		Log(this).error("Failed to parse input SMILES: '{0}'.", smiles);
		return false;
	}

	return true;
}


StructureSMILESPrintUnitTest::StructureSMILESPrintUnitTest(
	const std::string& name,
	const std::string& smiles
) noexcept :
	UnitTest(name + '_' + smiles),
	molecule(smiles)
{}

bool StructureSMILESPrintUnitTest::run()
{
	const auto smiles = molecule.toSMILES();
	const auto dumpedMolecule = MolecularStructure::create(smiles);
	if (not dumpedMolecule)
	{
		Log(this).error("Failed to parse dumped SMILES: '{0}'.", smiles);
		return false;
	}

	if (molecule == *dumpedMolecule)
	{
		Log(this).error("Equality check between dumped molecule: '{0}' and input failed.", smiles);
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
	auto map = instance.maximalMapTo(pattern).first;
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


StructureMassUnitTest::StructureMassUnitTest(
	const std::string& name,
	const std::string& smiles,
	const Amount<Unit::GRAM_PER_MOLE> expectedMass
) noexcept :
	UnitTest(name + '_' + smiles),
	expectedMass(expectedMass),
	molecule(smiles)
{}

bool StructureMassUnitTest::run()
{
	const auto mass = molecule.getMolarMass();
	if (mass != expectedMass)
	{
		Log(this).error("Actual molar mass: {0} is different from the expected mass: {1}.", mass.toString(), expectedMass.toString());
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

	// TODO: should validate parsing agains molar mass at least
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CN(C)C(=O)C1=CC=CC=C1");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "C1=CC=CC=C1R");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CC(=O)OC");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CC(=O)N(C)C");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "C1CC1");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CC1CCCC2CCCCC12");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CC1=CC2=C(NC=C2)C=C1");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "RC1=CC2=C(NC=C2)C=C1");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "OC1C2CC12");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CC2CCCC(C1CCCCC1)C2");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "CN1CC(C=C2C1CC3=CNC4=CC=CC2=C34)C(=O)O");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "[Mg](O)O");
	registerTest<StructureSMILESParseUnitTest>("parse_SMILES", "S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N");

	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CN(C)C(=O)C1=CC=CC=C1");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "C1=CC=CC=C1R");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CC(=O)OC");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CC(=O)N(C)C");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "C1CC1");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CC1CCCC2CCCCC12");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CC1=CC2=C(NC=C2)C=C1");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "RC1=CC2=C(NC=C2)C=C1");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "NC1C2=C1CN2");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "OC1C2CC12");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CC2CCCC(C1CCCCC1)C2");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "CN1CC(C=C2C1CC3=CNC4=CC=CC2=C34)C(=O)O");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "[Mg](O)O");
	registerTest<StructureSMILESPrintUnitTest>("print_SMILES", "S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N");

	registerTest<StructureEqualityUnitTest>("equality", "CN(C)C(=O)C1=CC=CC=C1", "C1=CC=CC=C1R", false);
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
	// TODO: This will result in a valence violation. Some check in substitute would be nice
	//registerTest<MolecularSubstitutionTest>("substitute", "CC(=C)C", "C1CCC1O", "OC1(CCC1)(=C)");

	registerTest<StructureMassUnitTest>("mass", "CN(C)C(=O)C1=CC=CC=C1", 149.193);
	registerTest<StructureMassUnitTest>("mass", "CC(=O)OC", 74.079);
	registerTest<StructureMassUnitTest>("mass", "C1CCCC1", 70.135);
	registerTest<StructureMassUnitTest>("mass", "CN1CC(C=C2C1CC3=CNC4=CC=CC2=C34)C(=O)O", 268.316);
	registerTest<StructureMassUnitTest>("mass", "[Mg](O)O", 58.319);

	registerTest<UnitTestSetup<AccessorTestCleanup>>("cleanup");
	Accessor<>::unsetDataStore();
}
