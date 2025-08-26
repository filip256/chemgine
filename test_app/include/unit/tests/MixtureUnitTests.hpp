#pragma once

#include "data/DataStore.hpp"
#include "mixtures/kinds/ForwardingContainer.hpp"
#include "mixtures/kinds/Reactor.hpp"
#include "unit/UnitTest.hpp"

class ReactorUnitTest : public UnitTest
{
protected:
    DumpContainer               dump;
    std::unique_ptr<Atmosphere> atmosphere;
    Reactor                     reactor;

    Amount<Unit::GRAM> getTotalSystemMass() const;

    void tick(const Amount<Unit::SECOND> timespan);

public:
    ReactorUnitTest(
        std::string&&                 name,
        const Amount<Unit::LITER>     maxVolume,
        const ContentInitializer&     content,
        std::unique_ptr<Atmosphere>&& atmosphere = Atmosphere::createDefaultAtmosphere(),
        FlagField<TickMode> tickMode             = TickMode::ENABLE_ALL) noexcept;

    ReactorUnitTest(
        std::string&&             name,
        const Amount<Unit::LITER> maxVolume,
        const ContentInitializer& content,
        FlagField<TickMode>       tickMode) noexcept;
};

class MassConservationUnitTest : public ReactorUnitTest
{
private:
    const float_h              threshold    = 6e-2;
    const uint32_t             ticks        = 512;
    const Amount<Unit::SECOND> tickTimespan = 1.0_s;

public:
    using ReactorUnitTest::ReactorUnitTest;

    bool run() override final;
};

class OverflowUnitTest : public ReactorUnitTest
{
private:
    const float_h threshold = 1.0e-3;

public:
    OverflowUnitTest(std::string&& name) noexcept;

    bool run() override final;
};

class DeterminismUnitTest : public ReactorUnitTest
{
private:
    const float_h              threshold    = 1e-3;
    const uint32_t             ticks        = 256;
    const Amount<Unit::SECOND> tickTimespan = 1.0_s;

public:
    using ReactorUnitTest::ReactorUnitTest;

    bool run() override final;
};

class BoilUnitTest : public ReactorUnitTest
{
private:
    const uint32_t             ticks        = 256;
    const Amount<Unit::SECOND> tickTimespan = 1.0_s;
    const Amount<Unit::JOULE>  energyStep   = 4000.0_J;

public:
    using ReactorUnitTest::ReactorUnitTest;

    bool run() override final;
};

class IncompatibleForwardingUnitTest : public ReactorUnitTest
{
private:
    SingleLayerMixture<LayerType::GASEOUS> source;

public:
    IncompatibleForwardingUnitTest(std::string&& name) noexcept;

    bool run() override final;
};

class ImplicitForwardingUnitTest : public ReactorUnitTest
{
private:
    ForwardingContainer source;

public:
    ImplicitForwardingUnitTest(std::string&& name) noexcept;

    bool run() override final;
};

class MixtureUnitTests : public UnitTestGroup
{
private:
    DataStore dataStore;

public:
    MixtureUnitTests(std::string&& name, const std::regex& filter, const std::string& defModulePath) noexcept;
};
