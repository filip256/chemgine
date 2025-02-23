#include "Performance/Tests/DefPerfTests.hpp"

DefPerfSetup::DefPerfSetup(
	std::string&& inputPath,
	std::string&& outputPath,
	const bool prettify
) noexcept:
	inputPath(std::move(inputPath)),
	outputPath(std::move(outputPath)),
	prettify(prettify)
{}

void DefPerfSetup::run()
{
	DataStore dataStore;
	Accessor<>::setDataStore(dataStore);

	LogBase::hide(LogType::ERROR);
	if (not dataStore.load(inputPath))
		Log(this).error("Failed to load: '{0}' during setup.", inputPath);
	LogBase::unhide();

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

void DefLoadPerfTest::preTask()
{
	Accessor<>::setDataStore(dataStore);
}

void DefLoadPerfTest::task()
{
	dataStore.load(path);
}

void DefLoadPerfTest::postTask()
{
	dataStore.clear();
	Accessor<>::unsetDataStore();
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

void DefDumpPerfTest::preTask()
{
	Accessor<>::setDataStore(dataStore);

	LogBase::hide(LogType::ERROR);

	if (not dataStore.load(inputPath))
		Log(this).error("Failed to load: '{0}' during setup.", inputPath);

	LogBase::unhide();
}

void DefDumpPerfTest::task()
{
	dataStore.dump(outputPath, prettify);
}

void DefDumpPerfTest::postTask()
{
	dataStore.clear();
	Accessor<>::unsetDataStore();
}

DefPerfTests::DefPerfTests(
	std::string&& name,
	const std::regex& filter,
	const std::string& inputPath
) noexcept :
	PerfTestGroup(std::move(name), filter)
{
	registerTest<PerfTestSetup<CreateDirTestSetup>>("setup", "./temp");

	registerTest<PerfTestSetup<DefPerfSetup>>("setup", Utils::copy(inputPath), "./temp/builtin.cdef", false);
	registerTest<DefLoadPerfTest>("load", std::chrono::seconds(20), "./temp/builtin.cdef");

	registerTest<PerfTestSetup<DefPerfSetup>>("setup", Utils::copy(inputPath), "./temp/builtin_pretty.cdef", true);
	registerTest<DefLoadPerfTest>("load_pretty", std::chrono::seconds(20), "./temp/builtin_pretty.cdef");

	// Disabled for now since dump does SSD writes
	// registerTest<DefDumpPerfTest>("dump", uint64_t(10), "./temp/builtin.cdef", "./temp/temp.cdef", false);
	// registerTest<DefDumpPerfTest>("dump_pretty", uint64_t(10), "./temp/builtin.cdef", "./temp/temp.cdef", false);

	registerTest<PerfTestSetup<RemoveDirTestSetup>>("cleanup", "./temp");
}
