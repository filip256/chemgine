#pragma once

#include "PerfTest.hpp"
#include "DataStore.hpp"

class DefPerfSetup : public PerfTestSetup
{
private:
	const std::string inputPath;
	const std::string outputPath;
	const bool prettify;

public:
	DefPerfSetup(
		std::string&& name,
		std::string&& inputPath,
		std::string&& outputPath,
		const bool prettify
	) noexcept;

	void task() override final;
};

class DefLoadPerfTest : public TimedTest
{
private:
	const std::string path;
	DataStore dataStore;

public:
	DefLoadPerfTest(
		std::string&& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		std::string&& path
	) noexcept;

	void setup() override final;
	void cleanup() override final;
	void task() override final;
};


class DefDumpPerfTest : public TimedTest
{
private:
	const std::string inputPath;
	const std::string outputPath;
	const bool prettify;

	DataStore dataStore;

public:
	DefDumpPerfTest(
		std::string&& name,
		const std::variant<uint64_t, std::chrono::nanoseconds> limit,
		std::string&& inputPath,
		std::string&& outputPath,
		const bool prettify
	) noexcept;

	void setup() override final;
	void cleanup() override final;
	void task() override final;
};


class DefPerfTests : public PerfTestGroup
{
public:
	DefPerfTests(
		std::string&& name,
		const std::regex& filter
	) noexcept;
};
