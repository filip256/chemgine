#include "DefPerfTests.hpp"

DefPerfSetup::DefPerfSetup(
	std::string&& name,
	std::string&& inputPath,
	std::string&& outputPath,
	const bool prettify
) noexcept:
	PerfTestSetup(std::move(name)),
	inputPath(std::move(inputPath)),
	outputPath(std::move(outputPath)),
	prettify(prettify)
{}

void DefPerfSetup::task()
{
	DataStore dataStore;
	Accessor<>::setDataStore(dataStore);

	LogBase::hide(LogType::ERROR);

	if (not dataStore.load(inputPath))
		Log(this).error("Failed to load: '{0}' during setup.", inputPath);

	LogBase::unhide;

	dataStore.dump(outputPath, prettify);
	Accessor<>::unsetDataStore();
}


DefLoadPerfTest::DefLoadPerfTest(
	std::string&& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	std::string&& path
) noexcept :
	TimedTest(std::move(name), limit),
	path(std::move(path))
{}

void DefLoadPerfTest::setup()
{
	Accessor<>::setDataStore(dataStore);
}

void DefLoadPerfTest::cleanup()
{
	dataStore.clear();
	Accessor<>::unsetDataStore();
}

void DefLoadPerfTest::task()
{
	dataStore.load(path);
}


DefDumpPerfTest::DefDumpPerfTest(
	std::string&& name,
	const std::variant<uint64_t, std::chrono::nanoseconds> limit,
	std::string&& inputPath,
	std::string&& outputPath,
	const bool prettify
) noexcept :
	TimedTest(std::move(name), limit),
	inputPath(std::move(inputPath)),
	outputPath(std::move(outputPath)),
	prettify(prettify)
{}

void DefDumpPerfTest::setup()
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide(LogType::ERROR);

	if (not dataStore.load(inputPath))
		Log(this).error("Failed to load: '{0}' during setup.", inputPath);

	LogBase::unhide;
}

void DefDumpPerfTest::cleanup()
{
	dataStore.clear();
	Accessor<>::unsetDataStore();
}

void DefDumpPerfTest::task()
{
	dataStore.dump(outputPath, prettify);
}


DefPerfTests::DefPerfTests(
	std::string&& name,
	const std::regex& filter
) noexcept :
	PerfTestGroup(std::move(name), filter)
{
	registerTest<DefPerfSetup>("setup", "./Data/builtin.cdef", "./TestFiles/builtin.cdef", false);
	registerTest<DefLoadPerfTest>("load", std::chrono::seconds(5), "./TestFiles/builtin.cdef");

	registerTest<DefPerfSetup>("setup", "./Data/builtin.cdef", "./TestFiles/builtin_pretty.cdef", true);
	registerTest<DefLoadPerfTest>("load_pretty", std::chrono::seconds(5), "./TestFiles/builtin_pretty.cdef");

	// Disabled for now since dump does SSD writes
	// registerTest<DefDumpPerfTest>("dump", uint64_t(10), "./TestFiles/builtin.cdef", "./TestFiles/temp.cdef", false);
	// registerTest<DefDumpPerfTest>("dump_pretty", uint64_t(10), "./TestFiles/builtin.cdef", "./TestFiles/temp.cdef", false);
}
