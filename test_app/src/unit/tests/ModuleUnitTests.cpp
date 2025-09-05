#include "unit/tests/ModuleUnitTests.hpp"

#include "unit/tests/PropertyUnitTests.hpp"
#include "utils/Path.hpp"

WaterPropertiesUnitTest::WaterPropertiesUnitTest(std::string&& name, const std::regex& filter) noexcept :
    UnitTestGroup(std::move(name), filter),
    molecule("O")
{
    registerTest<PropertyUnitTest<Molecule, Unit::CELSIUS, Unit::TORR>>(
        "mp",
        molecule,
        &Molecule::getMeltingPointAt,
        std::vector<DataPoint<Unit::CELSIUS, Unit::TORR>>({
            {0.0_C, 760.0_torr}
    }),
        1.0e-8);

    registerTest<PropertyUnitTest<Molecule, Unit::CELSIUS, Unit::TORR>>(
        "bp",
        molecule,
        &Molecule::getBoilingPointAt,
        std::vector<DataPoint<Unit::CELSIUS, Unit::TORR>>({
            {{118.1_C, 1400.0_torr},
             {107.8_C, 1000.0_torr},
             {104.8_C, 900.0_torr},
             {101.4_C, 800.0_torr},
             {100.0_C, 760.0_torr},
             {93.3_C, 600.0_torr},
             {89.0_C, 500.0_torr},
             {83.3_C, 400.0_torr},
             {75.6_C, 300.0_torr},
             {66.5_C, 200.0_torr},
             {51.9_C, 100.0_torr},
             {-16.9_C, 1.0_torr}}
    }),
        1.0e-5);

    Amount<Unit::GRAM_PER_MILLILITER> (Molecule::*getDensityAt)(Amount<Unit::CELSIUS>, Amount<Unit::TORR>) const =
        &Molecule::getDensityAt;
    registerTest<PropertyUnitTest<Molecule, Unit::GRAM_PER_MILLILITER, Unit::CELSIUS, Unit::TORR>>(
        "density",
        molecule,
        getDensityAt,
        std::vector<DataPoint<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS, Unit::TORR>>({
            {0.99989f,  0.0_C, 760.0_torr},
            {0.99992f,  1.0_C, 760.0_torr},
            {0.99996f,  3.0_C, 760.0_torr},
            {0.99995f,  4.0_C, 760.0_torr},
            {0.99993f,  5.0_C, 760.0_torr},
            {0.99965f, 10.0_C, 760.0_torr},
            {0.99567f, 30.0_C, 760.0_torr},
            {0.97176f, 80.0_C, 760.0_torr},
            {0.95909f, 99.0_C, 760.0_torr}
    }),
        1.0e-3);
}

ModuleUnitTest::ModuleUnitTest(
    const std::string& name, const std::regex& filter, const std::string& defModulePath) noexcept :
    UnitTestGroup(name + '_' + utils::extractFileName(defModulePath), filter)
{
    registerTest<UnitTestSetup<AccessorTestSetup>>("setup", dataStore, defModulePath);

    registerTest<WaterPropertiesUnitTest>("water");

    registerTest<UnitTestSetup<AccessorTestCleanup>>("cleanup");
}
