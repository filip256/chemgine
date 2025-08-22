#include "perf/tests/StructurePerfTests.hpp"

//
// StructureSMILESPerfTest
//

StructureSMILESPerfTest::StructureSMILESPerfTest(
    const std::string&                                   name,
    const std::variant<size_t, std::chrono::nanoseconds> limit,
    std::string&&                                        smiles) noexcept :
    TimedTest(name + '_' + smiles, limit),
    smiles(std::move(smiles))
{}

void StructureSMILESPerfTest::task()
{
    dontOptimize = (MolecularStructure::fromSMILES(smiles)->toSMILES().empty());
}

//
// StructurePerfTestBase
//

StructurePerfTestBase::StructurePerfTestBase(
    const std::string&                                   name,
    const std::variant<size_t, std::chrono::nanoseconds> limit,
    const std::string&                                   targetSmiles) noexcept :
    TimedTest(name + '_' + targetSmiles, limit),
    target(targetSmiles)
{}

//
// StructureComparePerfTestBase
//

StructureComparePerfTestBase::StructureComparePerfTestBase(
    const std::string&                                   name,
    const std::variant<size_t, std::chrono::nanoseconds> limit,
    const std::string&                                   targetSmiles,
    const std::string&                                   patternSmiles) noexcept :
    TimedTest(name + '_' + targetSmiles + '_' + patternSmiles, limit),
    target(targetSmiles),
    pattern(patternSmiles)
{}

//
// StructureEqualityPerfTest
//

void StructureEqualityPerfTest::task() { dontOptimize = (target == pattern); }

//
// StructureInequalityPerfTest
//

void StructureInequalityPerfTest::task() { dontOptimize = (target != pattern); }

//
// StructureAtomMapPerfTest
//

void StructureAtomMapPerfTest::task()
{
    dontOptimize = static_cast<bool>(target.mapTo(pattern, true).size());
}

//
// StructureMaximalAtomMapPerfTest
//

void StructureMaximalAtomMapPerfTest::task()
{
    dontOptimize = static_cast<bool>(target.maximalMapTo(pattern).first.size());
}

//
// StructureSubstitutionPerfTest
//

StructureSubstitutionPerfTest::StructureSubstitutionPerfTest(
    const std::string&                                   name,
    const std::variant<size_t, std::chrono::nanoseconds> limit,
    const std::string&                                   patternSmiles,
    const std::string&                                   instanceSmiles) noexcept :
    StructureComparePerfTestBase(name, limit, patternSmiles, instanceSmiles),
    atomMap(pattern.maximalMapTo(target).first)
{}

void StructureSubstitutionPerfTest::task()
{
    dontOptimize = MolecularStructure::addSubstituents(target, pattern, atomMap).isEmpty();
}

//
// StructureFundamentalCyclePerfTest
//

void StructureFundamentalCyclePerfTest::task()
{
    dontOptimize = static_cast<bool>(target.getFundamentalCycleBasis().size());
}

//
// StructureMinimalCyclePerfTest
//

void StructureMinimalCyclePerfTest::task()
{
    dontOptimize = static_cast<bool>(target.getMinimalCycleBasis().size());
}

//
// ASCIIPrintfTest
//

void ASCIIPrintTest::task()
{
    dontOptimize = static_cast<bool>(target.toASCII().toString().size());
}

//
// ASCIIParseTest
//

ASCIIParseTest::ASCIIParseTest(
    const std::string&                                   name,
    const std::variant<size_t, std::chrono::nanoseconds> limit,
    const std::string&                                   smiles) noexcept :
    TimedTest(name + '_' + smiles, limit),
    ascii(generateASCII(smiles))
{}

std::string ASCIIParseTest::generateASCII(const std::string& smiles)
{
    const auto molecule = MolecularStructure::fromSMILES(smiles);
    if (not molecule)
        Log<ASCIIParseTest>().fatal(
            "Failed to initialize test due to invalid SMILES: '{0}'.", smiles);

    return molecule->toASCII().toString().toString();
}

void ASCIIParseTest::task() { dontOptimize = MolecularStructure::fromASCII(ascii)->isEmpty(); }

//
// StructurePerfTests
//

StructurePerfTests::StructurePerfTests(
    std::string&& name, const std::regex& filter, const std::string& atomsFilePath) noexcept :
    PerfTestGroup(std::move(name), filter)
{
    Accessor<>::setDataStore(dataStore);

    LogBase::hide();
    dataStore.load(atomsFilePath);
    LogBase::unhide();

    registerTest<PerfTestSetup<AccessorTestSetup>>("setup", dataStore);

    registerTest<StructureSMILESPerfTest>(
        "SMILES", std::chrono::seconds(8), "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
    registerTest<StructureSMILESPerfTest>(
        "SMILES", std::chrono::seconds(8), "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
    registerTest<StructureSMILESPerfTest>(
        "SMILES", std::chrono::seconds(8), "S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N");
    registerTest<StructureSMILESPerfTest>(
        "SMILES",
        std::chrono::seconds(8),
        "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12");

    registerTest<StructureEqualityPerfTest>(
        "equality_true",
        std::chrono::seconds(5),
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CCC(C(CCCC)(CC(C(C)CC)CC)C)CC",
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CCC(C(CCCC)(CC(C(C)CC)CC)C)CC");
    registerTest<StructureEqualityPerfTest>(
        "equality_true",
        std::chrono::seconds(5),
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67",
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
    registerTest<StructureEqualityPerfTest>(
        "equality_true",
        std::chrono::seconds(5),
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
    registerTest<StructureEqualityPerfTest>(
        "equality_true",
        std::chrono::seconds(5),
        "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12",
        "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12");

    registerTest<StructureInequalityPerfTest>(
        "inequality_true",
        std::chrono::seconds(5),
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC",
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(CCC)C)CC");
    registerTest<StructureInequalityPerfTest>(
        "inequality_true",
        std::chrono::seconds(5),
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67",
        "C2CC1CC3C1C7C2CC6CCC4CC5CC3C45C67");
    registerTest<StructureInequalityPerfTest>(
        "inequality_false",
        std::chrono::seconds(5),
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");

    registerTest<StructureAtomMapPerfTest>(
        "map",
        std::chrono::seconds(5),
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC",
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
    registerTest<StructureAtomMapPerfTest>(
        "map",
        std::chrono::seconds(5),
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67",
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
    registerTest<StructureAtomMapPerfTest>(
        "map",
        std::chrono::seconds(5),
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");

    registerTest<StructureMaximalAtomMapPerfTest>(
        "maximal_map",
        std::chrono::seconds(10),
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC",
        "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC");
    registerTest<StructureMaximalAtomMapPerfTest>(
        "maximal_map",
        std::chrono::seconds(10),
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67",
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");
    registerTest<StructureMaximalAtomMapPerfTest>(
        "maximal_map",
        std::chrono::seconds(10),
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R");

    registerTest<StructureSubstitutionPerfTest>(
        "substitute", std::chrono::seconds(10), "C(=O)O", "CC(=O)OCCCCCC(CCCCCC(CCC)CCC)CCCCCC");
    registerTest<StructureSubstitutionPerfTest>(
        "substitute",
        std::chrono::seconds(10),
        "C1C2C(CC(C=O)CC2CCCC)CCC1CC(=O)OC",
        "CC(=O)OC(CCC2C1C(C(CC(CC=C)CC)CC2)C=O)C1");

    registerTest<StructureFundamentalCyclePerfTest>(
        "fundamental_cycle",
        std::chrono::seconds(10),
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
    registerTest<StructureFundamentalCyclePerfTest>(
        "fundamental_cycle", std::chrono::seconds(10), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");

    registerTest<StructureMinimalCyclePerfTest>(
        "minimal_cycle",
        std::chrono::seconds(10),
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C");
    registerTest<StructureMinimalCyclePerfTest>(
        "minimal_cycle", std::chrono::seconds(10), "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67");

    registerTest<ASCIIPrintTest>(
        "ascii_print", std::chrono::seconds(30), "CCN(CC)C(=O)C1CN(C2CC3=CNC4=CC=CC(=C34)C2=C1)C");
    registerTest<ASCIIPrintTest>(
        "ascii_print",
        std::chrono::seconds(30),
        "CC(=O)OC1=C2OC4C(OC(C)=O)C=CC3C5CC(C=C1)=C2C34CCN5C");
    registerTest<ASCIIParseTest>(
        "ascii_parse", std::chrono::seconds(10), "CCN(CC)C(=O)C1CN(C2CC3=CNC4=CC=CC(=C34)C2=C1)C");
    registerTest<ASCIIParseTest>(
        "ascii_parse",
        std::chrono::seconds(10),
        "CC(=O)OC1=C2OC4C(OC(C)=O)C=CC3C5CC(C=C1)=C2C34CCN5C");

    registerTest<PerfTestSetup<AccessorTestCleanup>>("cleanup");
    Accessor<>::unsetDataStore();
}
