#include "Unit/ModuleUnitTest.hpp"
#include "Unit/PropertyUnitTest.hpp"
#include "PathUtils.hpp"

WaterPropertiesUnitTest::WaterPropertiesUnitTest(
	std::string&& name,
	const std::regex& filter
) noexcept :
	UnitTestGroup(std::move(name), filter),
	molecule("O")
{
	registerTest<PropertyUnitTest<Molecule, AbsCelsius, Torr>>("mp", molecule, &Molecule::getMeltingPointAt,
		std::vector<DataPoint<AbsCelsius, Torr>>({ {0.0f * _AbsCelsius, 760.0f * _Torr} }), 1.0e-8);

	registerTest<PropertyUnitTest<Molecule, AbsCelsius, Torr>>("bp", molecule, &Molecule::getBoilingPointAt,
		std::vector<DataPoint<AbsCelsius, Torr>>({ {
			{118.1f * _AbsCelsius, 1400.0f * _Torr}, {107.8f * _AbsCelsius, 1000.0f * _Torr}, {104.8f * _AbsCelsius, 900.0f * _Torr}, {101.4f * _AbsCelsius, 800.0f * _Torr},
			{100.0f * _AbsCelsius, 760.0f * _Torr}, {93.3f * _AbsCelsius, 600.0f * _Torr}, {89.0f * _AbsCelsius, 500.0f * _Torr}, {83.3f * _AbsCelsius, 400.0f * _Torr},
			{75.6f * _AbsCelsius, 300.0f * _Torr}, {66.5f * _AbsCelsius, 200.0f * _Torr}, {51.9f * _AbsCelsius, 100.0f * _Torr}, {-16.9f * _AbsCelsius, 1.0f * _Torr} } }
	), 1.0e-5);

	//Quantity<GramPerMilliLiter>(Molecule:: * getDensityAt) (Quantity<Celsius>, Quantity<Torr>) const = &Molecule::getDensityAt;
	//registerTest<PropertyUnitTest<Molecule, GramPerMilliLiter, Celsius, Torr>>("density", molecule,
	//	&Molecule::getDensityAt, std::vector<DataPoint<GramPerMilliLiter, Celsius, Torr>>({
	//		{0.99989f * _GramPerMilliLiter, 0.0f * _Celsius, 760.0f * _Torr},
	//		{0.99992f * _GramPerMilliLiter, 1.0f * _Celsius, 760.0f * _Torr},
	//		{0.99996f * _GramPerMilliLiter, 3.0f * _Celsius, 760.0f * _Torr},
	//		{0.99995f * _GramPerMilliLiter, 4.0f * _Celsius, 760.0f * _Torr},
	//		{0.99993f * _GramPerMilliLiter, 5.0f * _Celsius, 760.0f * _Torr},
	//		{0.99965f * _GramPerMilliLiter, 10.0f * _Celsius, 760.0f * _Torr},
	//		{0.99567f * _GramPerMilliLiter, 30.0f * _Celsius, 760.0f * _Torr},
	//		{0.97176f * _GramPerMilliLiter, 80.0f * _Celsius, 760.0f * _Torr},
	//		{0.95909f * _GramPerMilliLiter, 99.0f * _Celsius, 760.0f * _Torr} }), 1.0e-3);
}


ModuleUnitTest::ModuleUnitTest(
	const std::string& name,
	const std::regex& filter,
	const std::string& defModulePath
) noexcept:
	UnitTestGroup(name + '_' + Utils::extractFileName(defModulePath), filter)
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide();
	dataStore.load(defModulePath);
	LogBase::unhide();

	registerTest<UnitTestSetup<AccessorTestSetup>>("setup", dataStore);

	registerTest<WaterPropertiesUnitTest>("water");

	registerTest<UnitTestSetup<AccessorTestCleanup>>("cleanup");
	Accessor<>::unsetDataStore();
}
