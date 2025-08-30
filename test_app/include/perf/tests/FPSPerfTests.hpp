#pragma once

#include "data/DataStore.hpp"
#include "labware/Lab.hpp"
#include "mixtures/kinds/Reactor.hpp"
#include "perf/PerfTest.hpp"

class FPSPerfTestBase : public TimedTest
{
protected:
    std::chrono::high_resolution_clock::time_point lastTick;
    Amount<Unit::SECOND>                           nextTickTimespan;

public:
    using TimedTest::TimedTest;

    void setup() override final;
    void preTask() override final;
    void postTask() override;
};

class ReactorFPSPerfTest : public FPSPerfTestBase
{
private:
    DumpContainer               dump;
    std::unique_ptr<Atmosphere> atmosphere;
    Reactor                     reactor;

public:
    ReactorFPSPerfTest(
        std::string&&                                        name,
        const std::variant<size_t, std::chrono::nanoseconds> limit,
        const ContentInitializer&                            content,
        std::unique_ptr<Atmosphere>&&                        atmosphere = Atmosphere::createDefaultAtmosphere(),
        FlagField<TickMode> tickMode                                    = TickMode::ENABLE_ALL) noexcept;

    ReactorFPSPerfTest(
        std::string&&                                        name,
        const std::variant<size_t, std::chrono::nanoseconds> limit,
        const ContentInitializer&                            content,
        FlagField<TickMode>                                  tickMode) noexcept;

    void task() override final;
    void postTask() override final;
};

class LabFPSPerfTest : public FPSPerfTestBase
{
private:
    Lab lab;

public:
    LabFPSPerfTest(std::string&& name, const std::variant<size_t, std::chrono::nanoseconds> limit, Lab&& lab) noexcept;

    void task() override final;
};

class FPSPerfTests : public PerfTestGroup
{
private:
    DataStore dataStore;

public:
    FPSPerfTests(std::string&& name, const std::regex& filter, const std::string& defModulePath) noexcept;
};
