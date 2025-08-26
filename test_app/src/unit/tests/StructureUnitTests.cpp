#include "unit/tests/StructureUnitTests.hpp"

#include "global/Charset.hpp"
#include "io/StringTable.hpp"
#include "molecules/MolecularStructure.hpp"
#include "utils/Bin.hpp"

#include <numeric>

namespace
{
namespace details
{

template <size_t I, typename CallableT, typename TupleT, typename... Idxs>
bool fuzzLoop(const TupleT& inputs, CallableT&& test, std::tuple<Idxs...> indices)
{
    const auto& currentStruct = std::get<I>(inputs);
    for (c_size i = 0; i < currentStruct.getNonImpliedAtomCount(); ++i) {
        const auto newIndices = std::tuple_cat(indices, std::make_tuple(i));
        if constexpr (I + 1 == std::tuple_size_v<TupleT>) {
            const auto success = [&]() {
                return std::apply([&](auto... idxs) {
                    return std::apply(
                        [&](const auto&... ss) { return test(MolecularStructure(ss.toSMILES(idxs))...); }, inputs);
                }, newIndices);
            }();
            if (not success)
                return false;
        }
        else {
            if (!fuzzLoop<I + 1>(inputs, test, newIndices))
                return false;
        }
    }

    return true;
}

}  // namespace details

// Runs the given test over multiple generated inputs obtained by fuzzing the original inputs.
// Fuzzing works by generating distinct SMILES notations for each input and parsing them back into
// molecules. The order of the original inputs is preserved for the fuzzed inputs. The test is also
// applied on the original inputs.
template <typename CallableT, typename... InTs>
bool fuzzTest(CallableT&& test, const InTs&... inputs)
{
    static_assert(sizeof...(InTs) >= 1, "fuzzTest(): No inputs to fuzz.");

    if (not test(inputs...))
        return false;

    const auto structsTuple = std::tie(inputs...);
    return details::fuzzLoop<0>(structsTuple, std::forward<CallableT>(test), std::tuple<>{});
}

}  // namespace

//
// StructureSMILESUnitTest
//

StructureSMILESUnitTest::StructureSMILESUnitTest(
    const std::string& name, std::string&& smiles, const Amount<Unit::GRAM_PER_MOLE> expectedMass) noexcept :
    UnitTest(name + '_' + smiles),
    expectedMass(expectedMass),
    smiles(std::move(smiles))
{}

bool StructureSMILESUnitTest::run()
{
    const auto parsedMolecule = MolecularStructure::fromSMILES(smiles);
    if (not parsedMolecule) {
        Log(this).error("Failed to parse input SMILES: '{0}'.", smiles);
        return false;
    }

    const auto actualMass = parsedMolecule->getMolarMass();
    if (not utils::floatEqual(actualMass.asStd(), expectedMass.asStd(), 0.005f)) {
        Log(this).error(
            "Parsed molecule's molar mass: {0} does not match the expected molar mass: {1}.",
            actualMass.toString(),
            expectedMass.toString());
        return false;
    }

    const auto printedSmiles = parsedMolecule->toSMILES();

    const auto printedMolecule = MolecularStructure::fromSMILES(printedSmiles);
    if (not printedMolecule) {
        Log(this).error("Failed to parse printed SMILES: '{0}'.", printedSmiles);
        return false;
    }

    if (*parsedMolecule != *printedMolecule) {
        Log(this).error("Equality check between printed molecule: '{0}' and input failed.", printedSmiles);
        return false;
    }

    return true;
}

//
// StructureEqualityUnitTest
//

StructureEqualityUnitTest::StructureEqualityUnitTest(
    const std::string& name, const std::string& targetSmiles, const std::string& patternSmiles, const bool expected) noexcept
    :
    UnitTest(name + '_' + targetSmiles + '_' + patternSmiles),
    expected(expected),
    target(targetSmiles),
    pattern(patternSmiles)
{}

bool StructureEqualityUnitTest::run()
{
    return fuzzTest([this](const auto& t, const auto& p) -> bool {
        const auto result = (t == p);
        if (result != expected) {
            Log(this).error(
                "Unexpected equality check result ({}) between target: '{}' and pattern: '{}'\n{}",
                result,
                t.toSMILES(),
                p.toSMILES(),
                t.toASCII().appendRight(p.toASCII(), std::string{' ', ASCII::LineV, ' '}).toString().toString());
            return false;
        }

        return true;
    }, target, pattern);
}

//
// StructureAtomMapUnitTest
//

StructureAtomMapUnitTest::StructureAtomMapUnitTest(
    const std::string& name, const std::string& targetSmiles, const std::string& patternSmiles, const bool expected) noexcept
    :
    UnitTest(name + '_' + targetSmiles + '_' + patternSmiles),
    expected(expected),
    target(targetSmiles),
    pattern(patternSmiles)
{}

bool StructureAtomMapUnitTest::run() { return (target.mapTo(pattern, true).size() > 0) == expected; }

//
// StructureMaximalAtomMapUnitTest
//

StructureMaximalAtomMapUnitTest::StructureMaximalAtomMapUnitTest(
    const std::string& name,
    const std::string& targetSmiles,
    const std::string& patternSmiles,
    const size_t       expectedSize) noexcept :
    UnitTest(name + '_' + targetSmiles + '_' + patternSmiles),
    expectedSize(expectedSize),
    target(targetSmiles),
    pattern(patternSmiles)
{}

bool StructureMaximalAtomMapUnitTest::run()
{
    const auto size = target.maximalMapTo(pattern).first.size();
    if (size != expectedSize) {
        Log(this).error("Actual map size: {0} is different from the expected size: {1}.", size, expectedSize);
        return false;
    }
    return true;
}

//
// StructureSubstitutionUnitTest
//

StructureSubstitutionUnitTest::StructureSubstitutionUnitTest(
    const std::string& name,
    const std::string& patternSmiles,
    const std::string& instanceSmiles,
    const std::string& expectedSmiles) noexcept :
    UnitTest(name + '_' + patternSmiles + '_' + instanceSmiles),
    pattern(patternSmiles),
    instance(instanceSmiles),
    expected(expectedSmiles)
{}

bool StructureSubstitutionUnitTest::run()
{
    const auto map = instance.maximalMapTo(pattern).first;
    if (map.empty()) {
        Log(this).error(
            "Generating the atom mapping between instance: '{0}' and pattern: '{1}' failed.",
            instance.toSMILES(),
            pattern.toSMILES());
        return false;
    }

    const auto result = MolecularStructure::addSubstituents(pattern, instance, map);
    if (result != expected) {
        Log(this).error(
            "Equality check between result: '{0}' and expected result: '{1}' failed.",
            result.toSMILES(),
            expected.toSMILES());
        return false;
    }

    return true;
}

//
// FundamentalCycleUnitTest
//

FundamentalCycleUnitTest::FundamentalCycleUnitTest(
    const std::string& name,
    const std::string& moleculeSmiles,
    const c_size       expectedCycleCount,
    const c_size       expectedTotalCyclicAtomCount) noexcept :
    UnitTest(name + '_' + moleculeSmiles),
    expectedCycleCount(expectedCycleCount),
    expectedTotalCyclicAtomCount(expectedTotalCyclicAtomCount),
    molecule(moleculeSmiles)
{}

bool FundamentalCycleUnitTest::run()
{
    const auto cycles = molecule.getFundamentalCycleBasis();
    if (cycles.size() != expectedCycleCount) {
        Log(this).error(
            "Actual cycle count: {0} is different from the expected count: {1}.", cycles.size(), expectedCycleCount);
        return false;
    }

    // Counting the total amount of distinct atoms participating in cycles should
    // be agnostic to canonicalization and the starting node of the algorithm.
    std::unordered_set<c_size> atomSet;
    atomSet.reserve(cycles.size());

    for (const auto& cycle : cycles)
        for (const auto atom : cycle) atomSet.emplace(atom->index);

    if (atomSet.size() != expectedTotalCyclicAtomCount) {
        Log(this).error(
            "Actual total cyclic atom count: {0} is different from the expected atom count: {1}.",
            atomSet.size(),
            expectedTotalCyclicAtomCount);
        return false;
    }

    return true;
}

//
// MinimalCycleUnitTest
//

MinimalCycleUnitTest::MinimalCycleUnitTest(
    const std::string&                   name,
    const std::string&                   moleculeSmiles,
    const c_size                         expectedTotalCyclicAtomCount,
    std::unordered_map<c_size, c_size>&& expectedCycleSizes) noexcept :
    UnitTest(name + '_' + moleculeSmiles),
    expectedTotalCyclicAtomCount(expectedTotalCyclicAtomCount),
    expectedCycleSizes(std::move(expectedCycleSizes)),
    molecule(moleculeSmiles)
{}

bool MinimalCycleUnitTest::run()
{
    const auto cycles = molecule.getMinimalCycleBasis();

    std::unordered_set<c_size> atomSet;
    atomSet.reserve(cycles.size());
    std::unordered_map<c_size, c_size> actualCycleSizes;
    actualCycleSizes.reserve(expectedCycleSizes.size());

    for (const auto& cycle : cycles) {
        for (const auto atom : cycle) atomSet.emplace(atom->index);

        if (auto it = actualCycleSizes.find(static_cast<c_size>(cycle.size())); it != actualCycleSizes.end())
            ++(it->second);
        else
            actualCycleSizes.emplace(static_cast<c_size>(cycle.size()), 1);
    }

    if (atomSet.size() != expectedTotalCyclicAtomCount) {
        Log(this).error(
            "Actual total cyclic atom count: {0} is different from the expected atom count: {1}.",
            atomSet.size(),
            expectedTotalCyclicAtomCount);
        return false;
    }

    if (actualCycleSizes != expectedCycleSizes) {
        StringTable diffTable({"Size", "Expected", "Actual"}, false);
        for (const auto& [refSize, refCount] : expectedCycleSizes) {
            if (const auto it = actualCycleSizes.find(refSize); it != actualCycleSizes.end()) {
                diffTable.addEntry({std::to_string(refSize), std::to_string(refCount), std::to_string(it->second)});
                actualCycleSizes.erase(it);
            }
            else
                diffTable.addEntry({std::to_string(refSize), std::to_string(refCount), "0"});
        }

        for (const auto& [actSize, actCount] : actualCycleSizes)
            diffTable.addEntry({std::to_string(actSize), "0", std::to_string(actCount)});

        Log(this).error("Actual cycle sizes differ from the expected sizes:\n{0}", diffTable.toString());
        return false;
    }

    return true;
}

//
// ASCIIPrintUnitTest
//

ASCIIPrintUnitTest::ASCIIPrintUnitTest(
    const std::string& name, const std::string& moleculeSmiles, const bool allowLinearCycleExpansion) noexcept :
    UnitTest(name + '_' + moleculeSmiles),
    allowLinearCycleExpansion(allowLinearCycleExpansion),
    molecule(moleculeSmiles)
{}

bool ASCIIPrintUnitTest::run()
{
    const auto ascii          = molecule.toASCII().toString().toString();
    const auto parsedMolecule = MolecularStructure::fromASCII(ascii);
    if (not parsedMolecule) {
        Log(this).error("Failed to parse input ASCII:\n{0}", ascii);
        return false;
    }

    if (*parsedMolecule != molecule) {
        Log(this).error(
            "Equality check between printed molecule: '{0}' and input: '{1}' failed:\n{2}",
            parsedMolecule->toSMILES(),
            molecule.toSMILES(),
            ascii);
        return false;
    }

    if (not allowLinearCycleExpansion && ascii.find('%') != std::string::npos) {
        Log(this).error("ASCII print is valid but contains unexpected linear cycle expansion closure:\n{0}", ascii);
        return false;
    }

    return true;
}

//
// MolBinUnitTest
//

MolBinUnitTest::MolBinUnitTest(const std::string& name, const std::string& moleculeSmiles) noexcept :
    UnitTest(name + '_' + moleculeSmiles),
    molecule(moleculeSmiles)
{}

bool MolBinUnitTest::run()
{
    std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
    molecule.toMolBin(stream);

    stream.seekg(0);
    const auto parsedMolecule = MolecularStructure::fromMolBin(stream);
    if (not parsedMolecule) {
        Log(this).error("Failed to parse input MolBin:\n{}", utils::toHex(stream.str(), '-', '\n'));
        return false;
    }

    if (*parsedMolecule != molecule) {
        Log(this).error(
            "Equality check between printed molecule: '{}' and input: '{}' failed.",
            parsedMolecule->toSMILES(),
            molecule.toSMILES());
        return false;
    }

    return true;
}

//
// StructureUnitTests
//

StructureUnitTests::StructureUnitTests(
    std::string&& name, const std::regex& filter, const std::string& atomsFilePath) noexcept :
    UnitTestGroup(std::move(name), filter)
{
    Accessor<>::setDataStore(dataStore);

    LogBase::hide();
    dataStore.load(atomsFilePath);
    LogBase::unhide();

    registerTest<UnitTestSetup<AccessorTestSetup>>("setup", dataStore);

    registerTest<StructureSMILESUnitTest>("SMILES", "CN(C)C(=O)C1=CC=CC=C1", 149.19f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C1=CC=CC=C1R", 77.11f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CC(=O)OC", 74.08f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CC(=O)N(C)C", 87.12f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C1CC1", 42.08f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CC1CCCC2CCCCC12", 152.28f);
    registerTest<StructureSMILESUnitTest>("SMILES", "RC1=CC2=C(NC=C2)C=C1", 116.15f);
    registerTest<StructureSMILESUnitTest>("SMILES", "NC1C2=C1CN2", 82.11f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C1C2C1C2", 54.09f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CC1C2C1C2", 68.12f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CC1C2C1N2", 69.11f);
    registerTest<StructureSMILESUnitTest>("SMILES", "OC1C2CC12", 70.09f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CNC1CC=C1C=C=CC3=C=CN3", 172.23f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CN4CCC15C=CC(O)C3OC2=C(O)C=CC(=CC1)C2C345", 285.34f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", 352.46f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CN1CC(C=C2C1CC3=CNC4=CC=CC2=C34)C(=O)O", 268.32f);
    registerTest<StructureSMILESUnitTest>("SMILES", "[Mg](O)O", 58.32f);
    registerTest<StructureSMILESUnitTest>("SMILES", "S(-O)(-O)(-O)(OCC)(OCCC(N(C)C)=O)C#N", 270.31f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CC2CCCC(C1CCCCC1)C2", 180.33f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", 254.42f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C1C2C13C24C38C4%10C79C56CC5C67C89%10", 150.18f);
    registerTest<StructureSMILESUnitTest>(
        "SMILES", "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12", 356.47f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C(C)C(CC(C(C)C(C(C)C)(C(C)C))(C(C)C)C)CC", 282.56f);
    registerTest<StructureSMILESUnitTest>("SMILES", "HC(H)(H)C(H)(H)C(H)(H)H", 44.1f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C%12C3CCC%123C", 82.15f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CNC1(CCCCC1=O)C2=CC=CC=C2Cl", 237.72f);
    registerTest<StructureSMILESUnitTest>("SMILES", "CN(C)CCC1=CNC2=C1C=C(C=C2)OC", 218.3f);
    registerTest<StructureSMILESUnitTest>("SMILES", "C1C2C3C4C1C15C6CC7C8C6C6CC8C8(C3CC4C618)C275", 260.38f);

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
    registerTest<StructureEqualityUnitTest>(
        "equality",
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R",
        false);
    registerTest<StructureEqualityUnitTest>(
        "equality",
        "COC4=C2C1=C([N]C=C1CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C4",
        "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R",
        false);
    registerTest<StructureEqualityUnitTest>(
        "equality", "C1C2C3C4C1C15C6CC7C8C6C6CC8C8(C3CC4C618)C275", "C1C2C3C4CC5C3C1C67C28C49C56C1CC9C2C8CC7C12", true);
    registerTest<StructureEqualityUnitTest>("equality", "[Cl]", "X", false);
    registerTest<StructureEqualityUnitTest>(
        "equality", "N2C1=CC=C(OC)C=C1C(CCN(C)C)C2", "N2C1=CC=C(OC)C=C1C(C2)CCN(C)C", true);

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
    registerTest<StructureAtomMapUnitTest>(
        "map",
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R",
        true);
    registerTest<StructureAtomMapUnitTest>(
        "map",
        "COC4=C2C1=C([N]C=C1CC3(CC(CC=C23)C(=O)N(C)C)NC(C)C)C=C4",
        "N(R)C14CC(CC=C1C2=C(OR)C=CC3=C2C(=C[N]3)C4)C(=O)N(R)R",
        true);
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
    registerTest<StructureSubstitutionUnitTest>(
        "substitute",
        "C1C2C(CC(C=O)CC2CCCC)CCC1CC(=O)OC",
        "CC(=O)OC(CCC2C1C(C(CC(CC=C)CC)CC2)C=O)C1",
        "O=CC2CC1C(CC(OC(=O)C)(CC1)CC(=O)OC)C(C2)C(CCC)CC=C");

    // TODO: This will result in a valence violation. Some check in substitute would be nice
    // registerTest<MolecularSubstitutionTest>("substitute", "CC(=C)C", "C1CCC1O", "OC1(CCC1)(=C)");

    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CC(=O)OC(C)C", 0, 0);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "OC1CCC1", 1, 4);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C1CCC(C)CC1", 1, 6);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CCC1CCC(O)CC1", 1, 6);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C1CC12CC2", 2, 5);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "CC2CC(CCCC)CC(C1CCCCC1)C2", 2, 12);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C1C2CC3CC23C1", 3, 7);
    registerTest<FundamentalCycleUnitTest>(
        "fundamental_cycle", "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C", 4, 16);
    registerTest<FundamentalCycleUnitTest>("fundamental_cycle", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", 7, 19);
    registerTest<FundamentalCycleUnitTest>(
        "fundamental_cycle", "C3=CC27CC18C=CC16C=C%10CCC%12C%11C=C5C=C4C(C=C2C3)C49C5=C(C6C789)C%10%11%12", 12, 28);

    registerTest<MinimalCycleUnitTest>("minimal_cycle", "CC(=O)OC(C)C", 0, std::unordered_map<c_size, c_size>());
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C1CCC(C)CC1",
        6,
        std::unordered_map<c_size, c_size>({
            {6, 1}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C1C(CCC2)C2CCC1",
        9,
        std::unordered_map<c_size, c_size>({
            {5, 1},
            {6, 1}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C1C2CC3CC23C1",
        7,
        std::unordered_map<c_size, c_size>({
            {3, 1},
            {4, 2}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "CCNC14CC(CC=C1C2=C(OC)C=CC3=C2C(=C[N]3)C4)C(=O)N(C)C",
        16,
        std::unordered_map<c_size, c_size>({
            {5, 1},
            {6, 3}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67",
        19,
        std::unordered_map<c_size, c_size>({
            {4, 3},
            {5, 1},
            {6, 3}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C12C3C1C23",
        4,
        std::unordered_map<c_size, c_size>({
            {3, 3}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C12C3C4C1C234",
        5,
        std::unordered_map<c_size, c_size>({
            {3, 4}
    }));
    registerTest<MinimalCycleUnitTest>(
        "minimal_cycle",
        "C12C3C4C1C5C2C3C45",
        8,
        std::unordered_map<c_size, c_size>({
            {4, 5}
    }));

    registerTest<ASCIIPrintUnitTest>("ASCII", "HH", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "O", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C=CCR", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Na]OC(C#N)([Br])S=O", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "N1OC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1=CC=CC=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "P1$S=NOC#C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "P1PP=PP=PP#P1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1C2CC12", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1CC2NC12", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1CC2CCCC(C1)C2", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C12C3C4C1C234", true);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1CN2CCCN3CCCN(C1)N23", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1CC(P2C(C)C(CC)CC2)C(SC)CC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1CCCCC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si]CCCC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1C[Si]CCC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si][Si]CCC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si]C[Si]CC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1C[Si]C[Si]C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si][Si][Si]CC1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si][Si]C[Si]C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si]C=[Si][Si]C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si][Si][Si][Si]C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1[Si][Si][Si][Si][Si]1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=CC=CC=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si]C=CC=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=C[Si]=CC=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si][Si]=CC=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si]C=[Si]C=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=C[Si]=C[Si]=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si][Si]=[Si]C=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si][Si]=C[Si]=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si]C=[Si][Si]=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si][Si]=[Si][Si]=C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "[Si]1=[Si][Si]=[Si][Si]=[Si]1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1C[Si]2N[Si]12", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C2=S13CCCCP1(CC2)CCCC3", true);
    registerTest<ASCIIPrintUnitTest>("ASCII", "CCOC1C(=NC)C(OC)C(OC)C1O", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "ICCS123CCC1C2CC3", true);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C2CC1CC3C1C7C2CCC6CC4CC5CC3C45C67", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C5CCC(C3CCS124(CCCC1)CCCC2CC34)CC5", true);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1(C)C(O)C(C)S(=CCCF)C(N)C(S)N(C=C(O)C)C(C)C(P)C1", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "C1CCCCCCCCCCCCCCCCS2S(CCCCCC1)PPPPPPPPPPPPPPPPPP2", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "COC1C=C2C(=CC=1)NCC2CCN(C)C", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "CN1C=NC2=C1C(=O)N(C(=O)N2C)C", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "CNC1(CCCCC1=O)C2=CC=CC=C2Cl", false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "CCN(CC)C(=O)C1CN(C2CC3=CNC4=CC=CC(=C34)C2=C1)C", false);
    registerTest<ASCIIPrintUnitTest>(
        "ASCII",
        "[Si][Si]N([Si][Si])[Si](=O)[Si]1[Si]N([Si]2[Si][Si]3=[Si]N[Si]4=[Si][Si]=[Si][Si](=[Si]34)"
        "[Si]2=[Si]1)[Si]",
        false);
    registerTest<ASCIIPrintUnitTest>("ASCII", "CC(=O)OC1=C2OC4C(O)C=CC3C5CC(C=C1)=C2C34CCN5C", true);

    registerTest<MolBinUnitTest>("MolBin", "HH");
    registerTest<MolBinUnitTest>("MolBin", "O");
    registerTest<MolBinUnitTest>("MolBin", "SNC");
    registerTest<MolBinUnitTest>("MolBin", "P1$S=NOC#C1");
    registerTest<MolBinUnitTest>("MolBin", "[Si]1=[Si][Si]=C[Si]=C1");
    registerTest<MolBinUnitTest>("MolBin", "CCN(CC)C(=O)C1CN(C2CC3=CNC4=CC=CC(=C34)C2=C1)C");

    registerTest<UnitTestSetup<AccessorTestCleanup>>("cleanup");
    Accessor<>::unsetDataStore();
}
