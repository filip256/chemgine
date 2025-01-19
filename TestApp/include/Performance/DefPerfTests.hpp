#pragma once

#include "PerfTest.hpp"
#include "DataStore.hpp"

class DefPerfSetup : public TestSetup
{
private:
	const std::string inputPath;
	const std::string outputPath;
	const bool prettify;

public:
	DefPerfSetup(
		std::string&& inputPath,
		std::string&& outputPath,
		const bool prettify
	) noexcept;

	void run() override final;
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

	void preTask() override final;
	void task() override final;
	void postTask() override final;
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

	void preTask() override final;
	void task() override final;
	void postTask() override final;
};


class DefPerfTests : public PerfTestGroup
{
public:
	DefPerfTests(
		std::string&& name,
		const std::regex& filter,
		const std::string& inputPath
	) noexcept;
};
