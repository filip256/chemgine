#include "mixtures/kinds/Reactor.hpp"

#include "data/DataStore.hpp"
#include "io/Log.hpp"

Reactor::Reactor(const Reactor& other) noexcept :
    MultiLayerMixture(static_cast<const MultiLayerMixture&>(other).makeCopy()),
    stirSpeed(other.stirSpeed),
    tickMode(other.tickMode)
{
    this->cachedReactions.reserve(other.cachedReactions.size());
    for (const auto& r : other.cachedReactions)
        this->cachedReactions.emplace(r.makeCopy());
}

Reactor::Reactor(
    const Ref<Atmosphere>     atmosphere,
    const Amount<Unit::LITER> maxVolume,
    const Ref<ContainerBase>  overflowTarget) noexcept :
    MultiLayerMixture(atmosphere, maxVolume, overflowTarget)
{}

Reactor::Reactor(const Ref<Atmosphere> atmosphere, const Amount<Unit::LITER> maxVolume) noexcept :
    Reactor(atmosphere, maxVolume, atmosphere)
{}

float_s Reactor::getInterLayerReactivityCoefficient(const Reactant& r1, const Reactant& r2) const
{
    if (r1.layer == r2.layer) {
        if (isSolidLayer(r1.layer))
            return static_cast<float_s>(0.0001);
        return static_cast<float_s>(1.0);
    }

    if (MultiLayerMixture::areAdjacentLayers(r1.layer, r2.layer) == false)
        return static_cast<float_s>(0.0);

    // TODO: take into account granularity, stirring, bubbling etc.
    if (isSolidLayer(r1.layer)) {
        if (isLiquidLayer(r2.layer))
            return static_cast<float_s>(0.5);  // S-L
        return static_cast<float_s>(0.01);     // S-G
    }

    if (isLiquidLayer(r1.layer)) {
        if (isSolidLayer(r2.layer))
            return static_cast<float_s>(0.5);  // L-S
        return static_cast<float_s>(0.1);      // L-G
    }

    if (isLiquidLayer(r2.layer))
        return static_cast<float_s>(0.1);  // G-L
    return static_cast<float_s>(0.01);     // G-S
}

float_s Reactor::getInterLayerReactivityCoefficient(const ReactantSet& reactants) const
{
    float_s result = 1.0;
    for (const auto& [_, r1] : reactants)
        for (const auto& [__, r2] : reactants) {
            result = std::min(getInterLayerReactivityCoefficient(r1, r2), result);
            if (result == 0.0)  // early return, coef is always positive
                return 0.0;
        }
    return result;
}

float_s Reactor::getCatalyticReactivityCoefficient(const ImmutableSet<Catalyst>& catalysts) const
{
    // TODO: take concentration into account
    for (size_t i = 0; i < catalysts.size(); ++i)
        if (getAmountOf(catalysts[i]) == 0.0)
            return 0.0;
    return 1.0;
}

void Reactor::findNewReactions()
{
    // TODO: optimize (perhaps a generator would be good)
    const auto reactants = utils::extractValues(content.getReactants());
    const auto arrangements =
        utils::getArrangementsWithRepetitions(reactants, dataAccessor.get().reactions.getMaxReactantCount());

    for (size_t i = 0; i < arrangements.size(); ++i) {
        const bool anyNew =
            std::any_of(arrangements[i].begin(), arrangements[i].end(), [](const Reactant& r) { return r.isNew; });

        if (anyNew == false)
            continue;

        auto newReactions = dataAccessor.get().reactions.findOccurringReactions(arrangements[i]);
        cachedReactions.merge(std::move(newReactions));
    }

    for (auto& [_, r] : content)
        r.isNew = false;
}

void Reactor::runReactions(const Amount<Unit::SECOND> timespan)
{
    for (const auto& r : cachedReactions) {
        auto speedCoef =
            r.getData()
                .getSpeedAt(r.getReactantTemperature(), getAmountOf(r.getReactants()).to<Unit::MOLE_RATIO>(totalMoles))
                .to<Unit::MOLE>(timespan) *
            totalVolume.asStd() *
            getInterLayerReactivityCoefficient(r.getReactants()) *
            getCatalyticReactivityCoefficient(r.getCatalysts());

        if (speedCoef == 0)
            continue;

        Log(this).trace("Applying reaction {0} with speed={1}.", r.getData().getHRTag(), speedCoef.toString());

        // if there isn't enough of a reactant, adjust the speed coefficient
        for (const auto& [_, i] : r.getReactants()) {
            const auto a = getAmountOf(i);
            if (a < i.amount * speedCoef)
                speedCoef = a / i.amount;
        }

        if (speedCoef == 0)
            continue;

        for (const auto& [_, i] : r.getReactants())
            MultiLayerMixture::add(i.mutate(-i.amount * speedCoef, *this));
        for (const auto& [_, i] : r.getProducts()) {
            const auto p = i.mutate(i.amount * speedCoef, *this);
            MultiLayerMixture::add(p.mutate(findLayerFor(p)));
        }

        MultiLayerMixture::add(r.getData().reactionEnergy.to<Unit::JOULE>(speedCoef), r.getReactants().any().layer);
    }
}

void Reactor::runLayerEnergyConduction(const Amount<Unit::SECOND> timespan)
{
    // TODO: find way to determine these based on molecular composition
    static const auto favourableC   = 0.000005_W;  // as relative conductivity
    static const auto unfavourableC = 0.000003_W;  // as relative conductivity

    for (auto& l : layers) {
        if (const auto above = getLayerAbove(l.first); above != LayerType::NONE) {
            const auto aboveLayer = layers.at(above);
            const auto diff       = (l.second.temperature - aboveLayer.temperature);
            if (diff == 0)
                continue;

            const auto diffE =
                diff > 0
                    ?
                    // molecules closer to the top usually have more
                    // energy than the layer avg. => favour up conversion
                    l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(l.second.moles).to<Unit::JOULE>(diff) *
                        favourableC.to<Unit::JOULE>(timespan)
                    : l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(aboveLayer.moles).to<Unit::JOULE>(diff) *
                          unfavourableC.to<Unit::JOULE>(timespan);

            MultiLayerMixture::add(diffE, above);
            MultiLayerMixture::add(-diffE, l.first);
        }

        if (const auto below = getLayerBelow(l.first); below != LayerType::NONE) {
            const auto belowLayer = layers.at(below);
            const auto diff       = (l.second.temperature - belowLayer.temperature);
            if (diff == 0)
                continue;

            const auto diffE =
                diff > 0
                    ?
                    // molecules closer to the bottom usually have less
                    // energy than the layer avg. => favour down conversion
                    l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(l.second.moles).to<Unit::JOULE>(diff) *
                        unfavourableC.to<Unit::JOULE>(timespan)
                    : l.second.getHeatCapacity().to<Unit::JOULE_PER_CELSIUS>(belowLayer.moles).to<Unit::JOULE>(diff) *
                          favourableC.to<Unit::JOULE>(timespan);

            MultiLayerMixture::add(diffE, below);
            MultiLayerMixture::add(-diffE, l.first);
        }
    }
}

void Reactor::consumePotentialEnergy()
{
    for (auto& l : layers) {
        l.second.convertTemporaryStateReactants();
        l.second.consumePotentialEnergy();
    }
}

void Reactor::addEnergy(const Amount<Unit::JOULE> energy) { MultiLayerMixture::addEnergy(energy); }

void Reactor::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
    MultiLayerMixture::add(molecule, amount);
}

FlagField<TickMode> Reactor::getTickMode() const { return tickMode; }

void Reactor::setTickMode(const FlagField<TickMode> mode) { tickMode.set(mode); }

void Reactor::tick(const Amount<Unit::SECOND> timespan)
{
    if (tickMode.has(TickMode::ENABLE_OVERFLOW))
        checkOverflow();

    if (tickMode.has(TickMode::ENABLE_NEGLIGIBLES))
        removeNegligibles();

    if (tickMode.has(TickMode::ENABLE_REACTIONS)) {
        findNewReactions();
        runReactions(timespan);
    }

    if (tickMode.has(TickMode::ENABLE_CONDUCTION))
        runLayerEnergyConduction(timespan);

    if (tickMode.has(TickMode::ENABLE_ENERGY))
        consumePotentialEnergy();
}

bool Reactor::hasSameState(const Reactor& other, const Amount<>::StorageType epsilon) const
{
    return this->pressure.equals(other.pressure, epsilon) &&
           this->totalMoles.equals(other.totalMoles, epsilon) &&
           this->totalMass.equals(other.totalMass, epsilon) &&
           this->totalVolume.equals(other.totalVolume, epsilon);
}

bool Reactor::hasSameContent(const Reactor& other, const Amount<>::StorageType epsilon) const
{
    return this->content.equals(other.content, epsilon);
}

bool Reactor::hasSameLayers(const Reactor& other, const Amount<>::StorageType epsilon) const
{
    auto l = LayerType::FIRST;
    while (l <= LayerType::LAST) {
        const bool hL = this->hasLayer(l);
        if (hL != other.hasLayer(l))
            return false;

        if (hL && (this->layers.at(l).equals(other.layers.at(l), epsilon)) == false)
            return false;
        ++l;
    }

    return true;
}

bool Reactor::isSame(const Reactor& other, const Amount<>::StorageType epsilon) const
{
    return this->hasSameState(other, epsilon) &&
           this->hasSameContent(other, epsilon) &&
           this->hasSameLayers(other, epsilon);
}

Reactor Reactor::makeCopy() const { return Reactor(*this); }
