#include "unit/tests/DefUnitTests.hpp"

#include "data/def/DefinitionParser.hpp"

DefUnitTest::DefUnitTest(std::string&& name, std::string&& defLine) noexcept :
    UnitTest(std::move(name)),
    defLine(std::move(defLine))
{}

bool DefUnitTest::run()
{
    LogBase::hide(LogType::WARN);
    bool success = true;

    const std::unordered_map<std::string, std::string> includeAliases;
    auto                                               def =
        def::parse<def::Object>(defLine, def::Location(getName(), 0), includeAliases, dataStore.outlineDefinitions);
    if (not def) {
        Log(this).error("Failed to parse definition: \n{0}", defLine);
        success = false;
    }

    if (success && not dataStore.addDefinition(std::move(*def))) {
        Log(this).error("Failed to store parsed definition: \0{0}", defLine);
        success = false;
    }

    LogBase::unhide();
    return success;
}

DefLoadUnitTest::DefLoadUnitTest(
    std::string&& name, DataStore& dataStore, std::string&& path, const bool expectedSuccess) noexcept :
    UnitTest(std::move(name)),
    dataStore(dataStore),
    path(std::move(path)),
    expectedSuccess(expectedSuccess)
{}

bool DefLoadUnitTest::run()
{
    LogBase::hide(expectedSuccess ? LogType::WARN : LogType::NONE);
    LogBase::nest();
    const auto success = dataStore.load(path);
    LogBase::unnest();
    LogBase::unhide();

    const auto estimatorCountBefore = dataStore.estimators.totalDefinitionCount();
    dataStore.estimators.dropUnusedEstimators();
    const auto estimatorCountAfter = dataStore.estimators.totalDefinitionCount();
    if (estimatorCountBefore != estimatorCountAfter) {
        Log(this).error(
            "Store contains {0} unused estimators after load completion.", estimatorCountBefore - estimatorCountAfter);
        return false;
    }

    if (success != expectedSuccess) {
        Log(this).error(
            "Actual load status: '{0}' does not match expected status: '{1}'.",
            path,
            def::print(success),
            def::print(expectedSuccess));
        return false;
    }

    return true;
}

DefCountUnitTest::DefCountUnitTest(
    std::string&& name, const DataStore& dataStore, const size_t expectedDefCount) noexcept :
    UnitTest(std::move(name)),
    dataStore(dataStore),
    expectedDefCount(expectedDefCount)
{}

bool DefCountUnitTest::run()
{
    const auto defCount = dataStore.totalDefinitionCount();
    if (defCount != expectedDefCount) {
        Log(this).error("Actual definition count: {0} does not match expected count: {1}.", defCount, expectedDefCount);
        return false;
    }

    return true;
}

DefDumpUnitTest::DefDumpUnitTest(
    std::string&& name, DataStore& dataStore, std::string&& path, const bool prettify) noexcept :
    UnitTest(std::move(name)),
    prettify(prettify),
    dataStore(dataStore),
    path(std::move(path))
{}

bool DefDumpUnitTest::run()
{
    dataStore.dump(path, prettify);
    return true;
}

DefClearUnitTest::DefClearUnitTest(std::string&& name, DataStore& dataStore) noexcept :
    UnitTest(std::move(name)),
    dataStore(dataStore)
{}

bool DefClearUnitTest::run()
{
    dataStore.clear();
    return dataStore.totalDefinitionCount() == 0;
}

DefUnitTests::DefUnitTests(std::string&& name, const std::regex& filter, const std::string& baseDefFilePath) noexcept :
    UnitTestGroup(std::move(name), filter)
{
    registerTest<UnitTestSetup<AccessorTestSetup>>("setup", dataStore);

    registerTest<DefClearUnitTest>("clear_base", dataStore);
    registerTest<DefLoadUnitTest>("load_base", dataStore, utils::copy(baseDefFilePath), true);

    registerTest<DefUnitTest>(
        "atom",
        "_atom:   Fe{"
        "name: iron,"
        "weight: 55.845_g,"
        "valences: {2,3}"
        "};");

    registerTest<DefUnitTest>(
        "radical",
        "_radical:* {"
        "name :any radical,"
        "matches:*"
        "};");

    registerTest<DefUnitTest>(
        "data_ool",
        "_data < d49 > : ( K ,torr  ) -> J/mol  {"
        "mode: LINEAR, values: { { 0.0, 1.0 } : 40700.0, { 1.0, 1.0 } : 40900.0 }"
        "};");

    registerTest<DefUnitTest>(
        "mol",
        "_mol: O {"
        "name:            water,"
        "melting_point : _ : atm->C{ values: { 1.0 : 0.0 } },"
        "boiling_point : _ : atm->C{ values: { 1.0 : 0.0 } },"
        "solid_density:   _: C->g/mL{ values: { -1.0 : 1.0 } },"
        "liquid_density : _ : C->g/mL{ values: { 1.0 : 0.9999017 } },"
        "solid_hc : _ : atm->J/(mol*C) { values: { 1.0 : 36.0 } },"
        "liquid_hc : _ : atm->J/(mol*C) {"
        "values: { 1.0 : 75.4840232 }"
        "},"
        "fusion_lh :      _data: (C, atm) -> J/mol {"
        "values: { { 0.0, 1.0 } : 6020.0 }"
        "},"
        "vaporization_lh: _data: (F, Pa) -> J/mol {"
        "values: { { 0.0, 1.0 } : 6020.0 }"
        "},"
        "sublimation_lh:  _data: (C, atm) -> J/mol {"
        "values: { { 0.0_F, 1.0 } : 6.0_kJ/mol }"
        "},"
        "rel_solubility:  _data: C -> 1 {"
        "values: { 1.0 : 1.0, 2.0 : 1.1 },"
        "},"
        "henry_const:    _data: C -> torr*(mol/mol) {"
        "values: { 0.0 : 1000.0 }"
        "},"
        "color:    { r: 0, g : 100, b : 255, intensity : 150 },"
        "};");

    registerTest<DefUnitTest>(
        "react",
        "_react: RC(=O)O[Me]  +  S(=O)(=O)(O)O ->  RC(=O)O  +  S(=O)(=O)(O)O[Me] {"
        "id : 213,"
        "name : acid displacement,"
        "speed_t : _ : C->mol/s{ base: _: C -> mol/s {"
        "values: { -INF : 0.0, -40.0 : 0.0625, -30.0 : 0.125, -20.0 : 0.25, -10.0 : 0.5 }"
        "},"
        "rebase_point: {20.0 : 0.3}"
        "},"
        "speed_c : _: mol/mol -> 1 { const: 1.0 },"
        "energy:    20.0_J/mol"
        "};");

    registerTest<DefUnitTest>(
        "react_cut",
        "_react: O=C(R)O + OC(R)R->O=C(R)OC(R)R + O{"
        "id:        214,"
        "name : cut - esterification,"
        "is_cut : true"
        "};");

    registerTest<DefUnitTest>(
        "lab_flask",
        "_lab: flask {"
        "id:      201,"
        "name : RBF,"
        "volume : 500_mL,"
        "ports : { INLET: { x: 31, y : 9, angle : 0.0_o }, CONTACT : {x: 31, y : 90, angle : "
        "0.0_o} },"
        "tx : ./data/builtin/tx/jrbf500.png"
        "};");

    registerTest<DefUnitTest>(
        "lab_adaptor",
        "_lab: adaptor{"
        "id:      301,"
        "name : 3 - way Adaptor,"
        "volume : 50_mL,"
        "ports : { OUTLET: { x: 6, y : 58, angle : 0.0_o }, OUTLET : {x: 36, y : 34, angle : "
        "290.0_o} },"
        "tx : ./data/builtin/tx/3way.png,"
        "};");

    registerTest<DefUnitTest>(
        "lab_heatsource",
        "_lab: heatsource{"
        "id:      401,"
        "name : Hotplate,"
        "power : 1.0_kW,"
        "ports : { CONTACT: { x: 41, y : 7, angle : 0.0_o } },"
        "tx : ./data/builtin/tx/hotplate_sh2.png,"
        "};");

    registerTest<DefUnitTest>(
        "lab_condenser",
        "_lab: condenser{"
        "id:       501,"
        "name : Liebig Condenser,"
        "volume : 200_mL,"
        "length : 0.3_m,"
        "effic : 0.5_/m,"
        "ports : { INLET: { x: 22, y : 12, angle : 0.0_o }, OUTLET : {x: 22, y : 158, angle : "
        "0.0_o} },"
        "tx : ./data/builtin/tx/liebig300.png,"
        "inner_mask : ./data/builtin/tx/liebig300_if.png,"
        "coolant_mask : ./data/builtin/tx/liebig300_cf.png,"
        "};");

    registerTest<DefClearUnitTest>("clear_base", dataStore);

    registerTest<UnitTestSetup<CreateDirTestSetup>>("setup", "./temp");

    registerTest<DefLoadUnitTest>("load_builtin", dataStore, "./data/builtin.cdef", true);
    registerTest<DefCountUnitTest>("count", dataStore, 213);
    registerTest<DefDumpUnitTest>("dump", dataStore, "./temp/builtin.cdef", false);
    registerTest<DefClearUnitTest>("clear", dataStore);
    registerTest<DefLoadUnitTest>("load", dataStore, "./temp/builtin.cdef", true);
    registerTest<DefCountUnitTest>("count", dataStore, 213);
    registerTest<DefDumpUnitTest>("dump_pretty", dataStore, "./temp/builtin_pretty.cdef", true);
    registerTest<DefLoadUnitTest>("reload", dataStore, "./temp/builtin_pretty.cdef", false);
    registerTest<DefCountUnitTest>("count", dataStore, 213);
    registerTest<DefClearUnitTest>("clear", dataStore);
    registerTest<DefLoadUnitTest>("load_pretty", dataStore, "./temp/builtin_pretty.cdef", true);
    registerTest<DefCountUnitTest>("count", dataStore, 213);
    registerTest<DefClearUnitTest>("clear", dataStore);

    registerTest<UnitTestSetup<RemoveDirTestSetup>>("cleanup", "./temp");
    registerTest<UnitTestSetup<AccessorTestCleanup>>("cleanup");
}
