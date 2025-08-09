#include "perf/tests/FPSPerfTests.hpp"
#include "labware/kinds/Flask.hpp"
#include "labware/kinds/Adaptor.hpp"
#include "labware/kinds/Condenser.hpp"
#include "labware/kinds/Heatsource.hpp"

void FPSPerfTestBase::setup()
{
	lastTick = std::chrono::high_resolution_clock::now();
}

void FPSPerfTestBase::preTask()
{
	// Run in real-time, as if tick() would be called on each frame.
	nextTickTimespan = std::max(
		(std::chrono::high_resolution_clock::now() - lastTick).count() / 1'000'000'000.0f,
		std::numeric_limits<float_s>::min());
}

void FPSPerfTestBase::postTask()
{
	lastTick = std::chrono::high_resolution_clock::now();
}


ReactorFPSPerfTest::ReactorFPSPerfTest(
	std::string&& name,
	const std::variant<size_t, std::chrono::nanoseconds> limit,
	const ContentInitializer& content,
	std::unique_ptr<Atmosphere>&& atmosphere,
	FlagField<TickMode> tickMode
) noexcept:
	FPSPerfTestBase(std::move(name), limit),
	dump(),
	atmosphere(std::move(atmosphere)),
	reactor(*this->atmosphere, 100.0_L)
{
	this->atmosphere->setOverflowTarget(dump);
	reactor.setTickMode(tickMode);
	for (const auto& m : content)
		reactor.add(m.first, m.second);
}

ReactorFPSPerfTest::ReactorFPSPerfTest(
	std::string&& name,
	const std::variant<size_t, std::chrono::nanoseconds> limit,
	const ContentInitializer& content,
	FlagField<TickMode> tickMode
) noexcept :
	ReactorFPSPerfTest(std::move(name), limit, content, Atmosphere::createDefaultAtmosphere(), tickMode)
{}

void ReactorFPSPerfTest::task()
{
	reactor.tick(nextTickTimespan);
}

void ReactorFPSPerfTest::postTask()
{
	atmosphere->tick(nextTickTimespan);
	FPSPerfTestBase::postTask();
}


LabFPSPerfTest::LabFPSPerfTest(
	std::string&& name,
	const std::variant<size_t, std::chrono::nanoseconds> limit,
	Lab&& lab
) noexcept :
	FPSPerfTestBase(std::move(name), limit),
	lab(std::move(lab))
{}

void LabFPSPerfTest::task()
{
	lab.tick(nextTickTimespan);
}


FPSPerfTests::FPSPerfTests(
	std::string&& name,
	const std::regex& filter,
	const std::string& defModulePath
) noexcept :
	PerfTestGroup(std::move(name), filter)
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide();
	dataStore.load(defModulePath);
	LogBase::unhide();

	registerTest<PerfTestSetup<AccessorTestSetup>>("setup", dataStore);

	registerTest<ReactorFPSPerfTest>("reactor_default", std::chrono::seconds(30), ContentInitializer({
		{Molecule("CC(=O)O"), 4.0_mol} }));

	Lab lab;
	auto& flask1 = lab.add<Flask>(201);
	flask1.add(Molecule("CC(=O)O"), 4.0_mol);
	lab.add<Adaptor>(301);
	lab.add<Adaptor>(302);
	lab.add<Heatsource>(401);
	lab.add<Condenser>(501);
	lab.add<Flask>(201);

	registerTest<LabFPSPerfTest>("lab_default", std::chrono::seconds(30), std::move(lab));

	registerTest<PerfTestSetup<AccessorTestCleanup>>("cleanup");
	Accessor<>::unsetDataStore();
}
