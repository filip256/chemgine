#include "Performance/StructurePerfTests.hpp"

StructureSMILESParsePerfTest::StructureSMILESParsePerfTest(
	const std::string& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	std::string&& smiles
) noexcept:
	TimedTest(name + '_' + smiles, limit),
	smiles(std::move(smiles)),
	molecule("")
{}
	
void StructureSMILESParsePerfTest::cleanup()
{
	molecule.clear();
}

void StructureSMILESParsePerfTest::task()
{
	dontOptimize = molecule.loadFromSMILES(smiles);
}


StructureOpsPerfTestBase::StructureOpsPerfTestBase(
	const std::string& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	const std::string& targetSmiles,
	const std::string& patternSmiles
) noexcept :
	TimedTest(name + '_' + targetSmiles + '_' + patternSmiles, limit),
	target(targetSmiles),
	pattern(patternSmiles)
{}


void StructureEqualityPerfTest::task()
{
	dontOptimize = (target == pattern);
}


void StructureInequalityPerfTest::task()
{
	dontOptimize = (target != pattern);
}


void StructureAtomMapPerfTest::task()
{
	dontOptimize = static_cast<bool>(target.mapTo(pattern, true).size());
}


void StructureMaximalAtomMapPerfTest::task()
{
	dontOptimize = static_cast<bool>(target.maximalMapTo(pattern).first.size());
}


StructureSubstitutionPerfTest::StructureSubstitutionPerfTest(
	const std::string& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	const std::string& patternSmiles,
	const std::string& intanceSmiles
) noexcept :
	StructureOpsPerfTestBase(name, limit, patternSmiles, intanceSmiles),
	atomMap(pattern.maximalMapTo(target).first)
{}

void StructureSubstitutionPerfTest::task()
{
	dontOptimize = MolecularStructure::addSubstituents(target, pattern, atomMap).isEmpty();
}


StructurePerfTests::StructurePerfTests(
	std::string&& name,
	const std::regex& filter,
	const std::string& atomsFilePath
) noexcept :
	PerfTestGroup(std::move(name), filter)
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide();
	dataStore.load(atomsFilePath);
	LogBase::unhide();

	registerTest<PerfTestSetup<AccessorTestSetup>>("setup", dataStore);

	registerTest<StructureSMILESParsePerfTest>("parse", std::chrono::seconds(2), "CCCCCCCCCCCCCCCCCCCC");
	registerTest<StructureSMILESParsePerfTest>("parse", std::chrono::seconds(2), "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
	registerTest<StructureSMILESParsePerfTest>("parse", std::chrono::seconds(2), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
	registerTest<StructureSMILESParsePerfTest>("parse", std::chrono::seconds(2), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
	registerTest<StructureSMILESParsePerfTest>("parse", std::chrono::seconds(2), "S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N");

	registerTest<StructureEqualityPerfTest>("equality_true", std::chrono::seconds(3), "CCCCCCCCCCCCCCCCCCCC", "CCCCCCCCCCCCCCCCCCCC");
	registerTest<StructureEqualityPerfTest>("equality_true", std::chrono::seconds(3), "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC", "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
	registerTest<StructureEqualityPerfTest>("equality_true", std::chrono::seconds(3), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
	registerTest<StructureEqualityPerfTest>("equality_true", std::chrono::seconds(3), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
	registerTest<StructureInequalityPerfTest>("equality_false", std::chrono::seconds(3), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "CCNC14CC(CC=C1C2=C(CC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");

	registerTest<StructureInequalityPerfTest>("inequality_true", std::chrono::seconds(3), "CCCCCCCCCCCCCCCCCCCC", "CCCCCCCCCCCCCCCCCCC");
	registerTest<StructureInequalityPerfTest>("inequality_true", std::chrono::seconds(3), "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC", "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(CCC)C)CC");
	registerTest<StructureInequalityPerfTest>("inequality_true", std::chrono::seconds(3), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", "C2CC1CC3C1C7C2CC6CCC4CC5CC3C45C67");
	registerTest<StructureInequalityPerfTest>("inequality_true", std::chrono::seconds(3), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "CCNC14CC(CC=C1C2=C(CC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
	registerTest<StructureInequalityPerfTest>("inequality_false",std::chrono::seconds(3), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");

	registerTest<StructureAtomMapPerfTest>("map", std::chrono::seconds(3), "CCCCCCCCCCCCCCCCCCCC", "CCCCCCCCCCCCCCCCCCCC");
	registerTest<StructureAtomMapPerfTest>("map", std::chrono::seconds(3), "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC", "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
	registerTest<StructureAtomMapPerfTest>("map", std::chrono::seconds(3), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
	registerTest<StructureAtomMapPerfTest>("map", std::chrono::seconds(3), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");

	registerTest<StructureMaximalAtomMapPerfTest>("maximal_map", std::chrono::seconds(5), "CCCCCCCCCCCCCCCCCCCC", "CCCCCCCCCCCCCCCCCCCC");
	registerTest<StructureMaximalAtomMapPerfTest>("maximal_map", std::chrono::seconds(5), "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC", "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
	registerTest<StructureMaximalAtomMapPerfTest>("maximal_map", std::chrono::seconds(10), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
	registerTest<StructureMaximalAtomMapPerfTest>("maximal_map", std::chrono::seconds(5), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");

	registerTest<StructureSubstitutionPerfTest>("substitute", std::chrono::seconds(2), "C(=O)O", "CC(=O)OC(C)C");

	registerTest<PerfTestSetup<AccessorTestCleanup>>("cleanup");
	Accessor<>::unsetDataStore();
}
