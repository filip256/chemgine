#include "MixtureUnitTests.hpp"
#include "UnitTestSetup.hpp"
#include "StringTable.hpp"

ReactorUnitTest::ReactorUnitTest(
	std::string&& name,
	const Amount<Unit::LITER> maxVolume,
	const ContentInitializer& contents,
	Atmosphere&& atmosphere,
	FlagField<TickMode> tickMode
) noexcept :
	UnitTest(std::move(name)),
	dump(),
	atmosphere(std::move(atmosphere)),
	reactor(this->atmosphere, maxVolume)
{
	this->atmosphere.setOverflowTarget(dump);
	reactor.setTickMode(tickMode);
	for (const auto& m : contents)
		reactor.add(m.first, m.second);
}

ReactorUnitTest::ReactorUnitTest(
	std::string&& name,
	const Amount<Unit::LITER> maxVolume,
	const ContentInitializer& contents,
	FlagField<TickMode> tickMode
) noexcept :
	ReactorUnitTest(std::move(name), maxVolume, contents, Atmosphere::createDefaultAtmosphere(), tickMode)
{}

Amount<Unit::GRAM> ReactorUnitTest::getTotalSystemMass() const
{
	return reactor.getTotalMass() + atmosphere.getTotalMass() + dump.getTotalMass();
}

void ReactorUnitTest::tick(const Amount<Unit::SECOND> timespan)
{
	reactor.tick(timespan);
	atmosphere.tick(timespan);
}


bool MassConservationUnitTest::run()
{
	const auto massBefore = getTotalSystemMass();
	for (size_t i = 0; i < ticks; ++i)
	{
		tick(tickTimespan);
		const auto massAfter = getTotalSystemMass();

		const auto error = std::abs((massAfter - massBefore).asStd());
		if (error > threshold)
		{
			Log(this).error("Mass loss: {0} exceeded the test threshold: {1} after tick {2}/{3}.",
				std::format("{:e}", error), std::format("{:e}", threshold), i + 1, ticks);
			return false;
		}
	}

	return true;
}


OverflowUnitTest::OverflowUnitTest(
	std::string&& name
) noexcept :
	ReactorUnitTest(std::move(name), 20.0_L, {})
{}

bool OverflowUnitTest::run()
{
	const auto containerVolume = reactor.getMaxVolume();
	auto error = std::abs((reactor.getTotalVolume() - containerVolume).asStd());
	if (error > threshold)
	{
		Log(this).error("Post-initialization content volume and container volume difference: {0} exceeded test threshold: {1}.",
			std::format("{:e}", error), std::format("{:e}", threshold));
		return false;
	}

	reactor.add(Molecule("O"), 700.0);
	const auto atmBefore = atmosphere.getTotalVolume();
	const auto reactorBefore = reactor.getTotalVolume();
	reactor.tick(1.0_s); // only tick reactor
	const auto atmAfter = atmosphere.getTotalVolume();
	const auto reactorAfter = reactor.getTotalVolume();

	error = std::abs((atmBefore + reactorBefore - atmAfter - reactorAfter).asStd());
	if (error > threshold)
	{
		Log(this).error("Overflow transfer loss: {0} exceeded test threshold: {1}.",
			std::format("{:e}", error), std::format("{:e}", threshold));
		return false;
	}

	error = std::abs((reactorAfter - containerVolume).asStd());
	if (error > threshold)
	{
		Log(this).error("Post-overflow volume and container volume difference: {0} exceeded test threshold: {1}.",
			std::format("{:e}", error), std::format("{:e}", threshold));
		return false;
	}

	return true;
}


bool DeterminismUnitTest::run()
{
	auto initial = reactor.makeCopy();
	auto copy = reactor.makeCopy();

	reactor.add(10.0_J);
	copy.add(10.0_J);
	reactor.tick(tickTimespan);
	copy.tick(tickTimespan);

	if (reactor.isSame(initial))
	{
		Log(this).error("Reactor is already in a stable state, test is inconclusive.");
		return false;
	}

	for (size_t i = 0; i < ticks; ++i)
	{
		auto success = true;
		auto error = std::abs((reactor.getPressure() - copy.getPressure()).asStd());
		if (error > threshold)
		{
			Log(this).error("Pressure difference: {0} exceeded the test threshold: {1} after tick {2}/{3}.",
				std::format("{:e}", error), std::format("{:e}", threshold), i + 1, ticks);
			success = false;
		}

		error = std::abs((reactor.getTotalMoles() - copy.getTotalMoles()).asStd());
		if (error > threshold)
		{
			Log(this).error("Moles difference: {0} exceeded the test threshold: {1} after tick {2}/{3}.",
				std::format("{:e}", error), std::format("{:e}", threshold), i + 1, ticks);
			success = false;
		}

		error = std::abs((reactor.getTotalMass() - copy.getTotalMass()).asStd());
		if (error > threshold)
		{
			Log(this).error("Mass difference: {0} exceeded the test threshold: {1} after tick {2}/{3}.",
				std::format("{:e}", error), std::format("{:e}", threshold), i + 1, ticks);
			success = false;
		}

		error = std::abs((reactor.getTotalVolume() - copy.getTotalVolume()).asStd());
		if (error > threshold)
		{
			Log(this).error("Volume difference: {0} exceeded the test threshold: {1} after tick {2}/{3}.",
				std::format("{:e}", error), std::format("{:e}", threshold), i + 1, ticks);
			success = false;
		}

		if (not reactor.hasSameContent(copy, threshold))
		{
			Log(this).error("Reactors have different contents after tick {0}/{1}.", i + 1, ticks);
			success = false;
		}

		if (not reactor.hasSameLayers(copy, threshold))
		{
			Log(this).error("Reactors have different layers after tick {0}/{1}.", i + 1, ticks);
			success = false;
		}

		if (not success)
			return false;

		reactor.add(10.0_J);
		copy.add(10.0_J);
		reactor.tick(tickTimespan);
		copy.tick(tickTimespan);
	}

	return true;
}


bool BoilUnitTest::run()
{
	const auto& source = reactor.getLayer(LayerType::POLAR);
	const auto& destination = reactor.getLayer(LayerType::GASEOUS);

	const auto sourceMaxTemp = source.getMaxAllowedTemperature();
	const auto nucleator = source.getHighNucleator();
	auto pastNucleatorAmount = reactor.getAmountOf(nucleator);
	auto pastSourceTemp = source.getTemperature();
	auto pastDestinationTemp = destination.getTemperature();

	bool success = true;
	uint8_t testPhase = 0;

	StringTable logTable({"Added Energy", "Phase", "Src. Temp.", "Dst. Temp.", "Nucleator"}, false);

	logTable.addEntry({
		Amount<Unit::JOULE>(0.0).toString(),
		std::to_string(testPhase),
		source.getTemperature().toString(),
		destination.getTemperature().toString(),
		reactor.getAmountOf(nucleator).toString() });

	for (size_t i = 0; i < ticks; ++i)
	{
		reactor.add(energyStep);
		reactor.tick(tickTimespan);

		logTable.addEntry({
			Amount<Unit::JOULE>(energyStep * (i + 1)).toString(),
			std::to_string(testPhase),
			source.getTemperature().toString(),
			destination.getTemperature().toString(),
			reactor.getAmountOf(nucleator).toString() });

		if (testPhase == 0) // heating up source to tp
		{
			const auto sTemp = source.getTemperature();
			if (sTemp == sourceMaxTemp)
			{
				++testPhase;
				pastSourceTemp = sTemp;
			}
			else if (sTemp >= sourceMaxTemp)
			{
				Log(this).error("Actual source layer temperature: {0} exceeded expected temperature: {1} in phase: {2}.",
					sTemp.toString(), sourceMaxTemp.toString(), testPhase);
				success = false;
				break;
			}

			const auto dTemp = destination.getTemperature();;
			if (dTemp != pastDestinationTemp)
			{
				Log(this).error("Destination layer temperature: {0} changed in phase: {1}.",
					dTemp.toString(), testPhase);
				success = false;
				break;
			}
			pastDestinationTemp = dTemp;
		}
		else if (testPhase == 1) // heating up dest to tp
		{
			const auto dTemp = destination.getTemperature();
			if (dTemp == sourceMaxTemp)
			{
				++testPhase;
				pastDestinationTemp = dTemp;
			}
			else if (dTemp >= sourceMaxTemp)
			{
				Log(this).error("Actual destination layer temperature: {0} exceeded expected temperature: {1} in phase: {2}.",
					dTemp.toString(), sourceMaxTemp.toString(), testPhase);
				success = false;
				break;
			}

			const auto sTemp = source.getTemperature();;
			if (sTemp != pastSourceTemp)
			{
				Log(this).error("Source layer temperature: {0} changed in phase: {1}.",
					dTemp.toString(), testPhase);
				break;
			}
			pastSourceTemp = sTemp;
		}
		else if (testPhase == 2) // transfering all the nucleator
		{
			const auto nMoles = reactor.getAmountOf(nucleator);
			if (nMoles >= pastNucleatorAmount)
			{
				Log(this).error("Failed to transfer nucleator: '{0}' in phase : {1}.",
					nucleator.molecule.getStructure().toSMILES(), testPhase);
				success = false;
				break;
			}
			pastNucleatorAmount = nMoles;

			const auto dTemp = destination.getTemperature();;
			if (dTemp != pastDestinationTemp)
			{
				Log(this).error("Destination layer temperature: {0} changed in phase: {1}.",
					dTemp.toString(), testPhase);
				success = false;
				break;
			}
			pastDestinationTemp = dTemp;

			if (nMoles == 0.0_mol)
				++testPhase;
		}
		else if (testPhase == 3) // full heat conversion
		{
			const auto dTemp = destination.getTemperature();
			if (dTemp <= pastDestinationTemp)
			{
				Log(this).error("Destination layer temperature: {0} did not increase in phase: {1}.",
					dTemp.toString(), testPhase);
				success = false;
				break;
			}
			pastDestinationTemp = dTemp;
			if (const auto sTemp = source.getTemperature(); not sTemp.isInfinity())
			{
				Log(this).error("Source layer temperature: {0} was not infinity in phase {1}.",
					sTemp.toString(), testPhase);
				success = false;
				break;
			}
		}
	}

	if(success && testPhase != 3)
	{
		Log(this).error("Not all test phases were reached, current phase: {0}.", testPhase);
		success = false;
	}


	if (not success)
	{
		Log(this).debug("UnitTest results:\n{0}", logTable.toString());
		return false;
	}

	return true;
}


IncompatibleForwardingUnitTest::IncompatibleForwardingUnitTest(
	std::string&& name
) noexcept :
	ReactorUnitTest(std::move(name), 1.0_L, {}),
	source(1.0_C, 760.0_torr, { { Molecule("N#N"), 78.084_mol } }, 0.5_L, dump)
{
	source.setIncompatibilityTarget(LayerType::POLAR, reactor);
}

bool IncompatibleForwardingUnitTest::run()
{
	const auto water = Reactant(Molecule("O"), LayerType::POLAR, 1.0_mol);
	const auto oxygen = Reactant(Molecule("O=O"), LayerType::GASEOUS, 1.0_mol);

	source.add(water);
	if (reactor.getAmountOf(water) != water.amount)
	{
		Log(this).error("Incompatible reactant: '{0}' was not forwarded.",
			water.molecule.getStructure().toSMILES());
		return false;
	}

	const auto molesBefore = reactor.getTotalMoles();
	source.add(oxygen);
	if (reactor.getTotalMoles() != molesBefore)
	{
		Log(this).error("Compatible reactant: '{0}' was forwarded.",
			oxygen.molecule.getStructure().toSMILES());
		return false;
	}

	return true;
}


ImplicitForwardingUnitTest::ImplicitForwardingUnitTest(
	std::string&& name
) noexcept :
	ReactorUnitTest(std::move(name), 1.0_L, {}),
	source({ { [](const Reactant& reactant) -> bool { return reactant.molecule.getMolarMass() < 20.0_g; }, Ref<ContainerBase>(reactor) } },
		dump)
{}

bool ImplicitForwardingUnitTest::run()
{
	const auto water = Reactant(Molecule("O"), LayerType::POLAR, 1.0_mol);
	const auto oxygen = Reactant(Molecule("O=O"), LayerType::GASEOUS, 1.0_mol);

	source.add(water);
	if (reactor.getAmountOf(water) != water.amount)
	{
		Log(this).error("Incompatible reactant: '{0}' was not forwarded.",
			water.molecule.getStructure().toSMILES());
		return false;
	}

	const auto molesBefore = reactor.getTotalMoles();
	source.add(oxygen);
	if (reactor.getTotalMoles() != molesBefore)
	{
		Log(this).error("Compatible reactant: '{0}' was forwarded.",
			oxygen.molecule.getStructure().toSMILES());
		return false;
	}

	return true;
}


MixtureUnitTests::MixtureUnitTests(
	std::string&& name,
	const std::regex& filter,
	const std::string& defModulePath
) noexcept:
	UnitTestGroup(std::move(name), filter)
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide();
	dataStore.load(defModulePath);
	LogBase::unhide();

	registerTest<AccessorUnitTestSetup>("setup", dataStore);

	registerTest<MassConservationUnitTest>("mass_conservation_0", 1.0_L, ContentInitializer({
		{Molecule("HH"), 2.0_mol}, {Molecule("CC=C"), 2.0_mol}, {Molecule("CC(=O)OCC"), 2.0_mol}, {Molecule("O"), 3.0_mol} }));
	
	registerTest<OverflowUnitTest>("overflow");

	registerTest<DeterminismUnitTest>("determinism_0", 1.0_L, ContentInitializer({
		{Molecule("CC(=O)O"), 2.0_mol}, {Molecule("OCC"), 3.0_mol} }));

	registerTest<BoilUnitTest>("boil_0", 0.1_L, ContentInitializer({
		{Molecule("O"), 5.4_mol} }), FlagField(TickMode::ENABLE_ALL) - TickMode::ENABLE_CONDUCTION);

	registerTest<IncompatibleForwardingUnitTest>("forward_incompatible");
	registerTest<ImplicitForwardingUnitTest>("forward_implicit");

	registerTest<AccessorUnitTestCleanup>("cleanup");
	Accessor<>::unsetDataStore();
}
