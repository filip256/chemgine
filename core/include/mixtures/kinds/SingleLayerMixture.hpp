#pragma once

#include "data/values/Constants.hpp"
#include "io/Log.hpp"
#include "mixtures/ContentInitializer.hpp"
#include "mixtures/Layer.hpp"
#include "mixtures/kinds/DumpContainer.hpp"
#include "mixtures/kinds/Mixture.hpp"
#include "structs/FlagField.hpp"
#include "structs/Ref.hpp"

#include <unordered_map>

template <LayerType L>
class SingleLayerMixture : public Mixture
{
protected:
    Layer                                             layer;
    Amount<Unit::TORR>                                pressure;
    const Amount<Unit::LITER>                         maxVolume;
    Ref<ContainerBase>                                overflowTarget = DumpContainer::GlobalDumpContainer;
    std::unordered_map<LayerType, Ref<ContainerBase>> incompatibilityTargets;

    void addToLayer(const Reactant& reactant);

    void add(const Amount<Unit::JOULE> heat, const LayerType l) override final;

    LayerType findLayerFor(const Reactant& reactant) const override final;

    void removeNegligibles();
    void checkOverflow();
    void consumePotentialEnergy();

    void scaleToVolume(const Amount<Unit::LITER> volume);

    SingleLayerMixture(const SingleLayerMixture& other) noexcept;

public:
    SingleLayerMixture(
        const Amount<Unit::CELSIUS> temperature,
        const Amount<Unit::TORR>    pressure,
        const ContentInitializer&   contentInitializer,
        const Amount<Unit::LITER>   maxVolume,
        const Ref<ContainerBase>    overflowTarget) noexcept;

    // SingleLayerMixture(SingleLayerMixture&&) = default;

    const Layer&                         getLayer() const;
    Amount<Unit::CELSIUS>                getLayerTemperature() const;
    Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity() const;
    Amount<Unit::JOULE_PER_CELSIUS>      getLayerTotalHeatCapacity() const;
    Amount<Unit::JOULE_PER_MOLE>         getLayerKineticEnergy() const;
    Polarity                             getLayerPolarity() const;
    Color                                getLayerColor() const;

    Ref<ContainerBase> getOverflowTarget() const override final;
    void               setOverflowTarget(const Ref<ContainerBase> target) override final;
    void               setIncompatibilityTarget(const LayerType layerType, const Ref<ContainerBase> target);
    void               setIncompatibilityTargets(const FlagField<LayerType> layerTypes, const Ref<ContainerBase> target);
    void               setAllIncompatibilityTargets(const Ref<ContainerBase> target);
    Ref<ContainerBase> getIncompatibilityTarget(const LayerType layerType) const;

    void add(const Reactant& reactant) override final;
    void add(const Molecule& molecule, const Amount<Unit::MOLE> amount) override final;
    void addEnergy(const Amount<Unit::JOULE> energy) override final;

    Amount<Unit::LITER> getMaxVolume() const;

    Amount<Unit::TORR>  getPressure() const override final;
    Amount<Unit::MOLE>  getTotalMoles() const override final;
    Amount<Unit::GRAM>  getTotalMass() const override final;
    Amount<Unit::LITER> getTotalVolume() const override final;

    const Layer&                         getLayer(const LayerType l) const override final;
    Amount<Unit::CELSIUS>                getLayerTemperature(const LayerType l) const override final;
    Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType l) const override final;
    Amount<Unit::JOULE_PER_CELSIUS>      getLayerTotalHeatCapacity(const LayerType layer) const override final;
    Amount<Unit::JOULE_PER_MOLE>         getLayerKineticEnergy(const LayerType l) const override final;
    Polarity                             getLayerPolarity(const LayerType layer) const override final;
    Color                                getLayerColor(const LayerType layer) const override final;

    bool isEmpty() const override final;

    void copyContentTo(const Ref<ContainerBase> destination, const Amount<Unit::LITER> volume) const;
    void moveContentTo(const Ref<ContainerBase> destination, const Amount<Unit::LITER> volume);

    SingleLayerMixture<L> makeCopy() const;
};

template <LayerType L>
SingleLayerMixture<L>::SingleLayerMixture(const SingleLayerMixture& other) noexcept :
    Mixture(other),
    layer(other.layer.makeCopy(*this)),
    pressure(other.pressure),
    maxVolume(other.maxVolume),
    overflowTarget(other.overflowTarget),
    incompatibilityTargets(other.incompatibilityTargets)
{}

template <LayerType L>
SingleLayerMixture<L>::SingleLayerMixture(
    const Amount<Unit::CELSIUS> temperature,
    const Amount<Unit::TORR>    pressure,
    const ContentInitializer&   contentInitializer,
    const Amount<Unit::LITER>   maxVolume,
    const Ref<ContainerBase>    overflowTarget) noexcept :
    layer(*this, L, temperature),
    pressure(pressure),
    maxVolume(maxVolume),
    overflowTarget(overflowTarget)
{
    incompatibilityTargets.reserve(getLayerCount());
    for (LayerType i = LayerType::FIRST; i <= LayerType::LAST; ++i)
        if (i != L)
            incompatibilityTargets.emplace(std::make_pair(i, Ref(DumpContainer::GlobalDumpContainer)));

    for (const auto& [m, a] : contentInitializer) add(Reactant(m, L, a, *this));
    scaleToVolume(maxVolume);
}

template <LayerType L>
void SingleLayerMixture<L>::addToLayer(const Reactant& reactant)
{
    layer.moles  += reactant.amount;
    layer.mass   += reactant.getMass();
    layer.volume += reactant.getVolume();

    if (reactant.isNew)
        layer.setIfNucleator(reactant);
    else if (content.contains(reactant) == false)
        layer.unsetIfNucleator(reactant);
}

template <LayerType L>
void SingleLayerMixture<L>::add(const Amount<Unit::JOULE> heat, const LayerType l)
{
    if (l != L)
        return;

    layer.potentialEnergy += heat;
}

template <LayerType L>
LayerType SingleLayerMixture<L>::findLayerFor(const Reactant& reactant) const
{
    const auto newAgg   = reactant.getAggregationAt(layer.temperature);
    const auto polarity = reactant.molecule.getPolarity();
    const auto density  = reactant.molecule.getDensityAt(layer.temperature, pressure);
    return getLayerType(newAgg, polarity.getPartitionCoefficient() > 1.0, density > 1.0);
}

template <LayerType L>
void SingleLayerMixture<L>::removeNegligibles()
{
    bool removedAny = false;
    for (auto r = content.begin(); r != content.end();) {
        if (r->second.amount < Constants::MOLAR_EXISTENCE_THRESHOLD) {
            const auto temp = r->second.mutate(-r->second.amount);
            r               = content.erase(r);
            addToLayer(temp);
            removedAny = true;
            continue;
        }

        ++r;
    }

    if (removedAny == false)
        return;

    for (const auto& [_, r] : content) {
        layer.setIfNucleator(r);
    }
}

template <LayerType L>
void SingleLayerMixture<L>::checkOverflow()
{
    if (maxVolume.isInfinity())
        return;

    const auto overflow = layer.volume - maxVolume;
    if (overflow <= 0.0)
        return;

    moveContentTo(overflowTarget, overflow);
}

template <LayerType L>
void SingleLayerMixture<L>::consumePotentialEnergy()
{
    layer.convertTemporaryStateReactants();
    layer.consumePotentialEnergy();
}

template <LayerType L>
void SingleLayerMixture<L>::scaleToVolume(const Amount<Unit::LITER> volume)
{
    const auto difV = volume - layer.volume;
    if (difV == 0.0)
        return;

    // is copy content to self safe?
    // YES, no realloc occurs, only the amounts change
    copyContentTo(*this, difV);
}

template <LayerType L>
const Layer& SingleLayerMixture<L>::getLayer() const
{
    return layer;
}

template <LayerType L>
Ref<ContainerBase> SingleLayerMixture<L>::getOverflowTarget() const
{
    return overflowTarget;
}

template <LayerType L>
void SingleLayerMixture<L>::setOverflowTarget(const Ref<ContainerBase> target)
{
    if (this == &*target)
        Log(this).warn("Overflow target set to self.");

    overflowTarget = target;
}

template <LayerType L>
void SingleLayerMixture<L>::setIncompatibilityTarget(const LayerType layerType, const Ref<ContainerBase> target)
{
    if (layerType == L) {
        Log(this).error("Tried to set incompatibility target for its own layer.");
        return;
    }

    if (this == &*target)
        Log(this).warn("Incompatibility target set to self.");

    incompatibilityTargets.at(layerType) = target;
}

template <LayerType L>
void SingleLayerMixture<L>::setIncompatibilityTargets(const FlagField<LayerType> layerTypes, const Ref<ContainerBase> target)
{
    for (auto l = layerTypes.begin(); l != layerTypes.end(); ++l) setIncompatibilityTarget(*l, target);
}

template <LayerType L>
void SingleLayerMixture<L>::setAllIncompatibilityTargets(const Ref<ContainerBase> target)
{
    setIncompatibilityTargets(FlagField(LayerType::ANY) - L, target);
}

template <LayerType L>
Ref<ContainerBase> SingleLayerMixture<L>::getIncompatibilityTarget(const LayerType layerType) const
{
    if (layerType == L)
        return NullRef;

    return incompatibilityTargets.at(layerType);
}

template <LayerType L>
void SingleLayerMixture<L>::add(const Reactant& reactant)
{
    if (reactant.layer != L) {
        incompatibilityTargets.at(reactant.layer)->add(reactant);
        return;
    }

    content.add(reactant);
    addToLayer(reactant.mutate(*this));
}

template <LayerType L>
void SingleLayerMixture<L>::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
    add(Reactant(molecule, L, amount, *this));
}

template <LayerType L>
void SingleLayerMixture<L>::addEnergy(const Amount<Unit::JOULE> energy)
{
    add(energy, L);
}

template <LayerType L>
Amount<Unit::LITER> SingleLayerMixture<L>::getMaxVolume() const
{
    return maxVolume;
}

template <LayerType L>
Amount<Unit::TORR> SingleLayerMixture<L>::getPressure() const
{
    return pressure;
}

template <LayerType L>
Amount<Unit::MOLE> SingleLayerMixture<L>::getTotalMoles() const
{
    return layer.moles;
}

template <LayerType L>
Amount<Unit::GRAM> SingleLayerMixture<L>::getTotalMass() const
{
    return layer.mass;
}

template <LayerType L>
Amount<Unit::LITER> SingleLayerMixture<L>::getTotalVolume() const
{
    return layer.volume;
}

template <LayerType L>
const Layer& SingleLayerMixture<L>::getLayer(const LayerType) const
{
    return layer;
}

template <LayerType L>
Amount<Unit::CELSIUS> SingleLayerMixture<L>::getLayerTemperature() const
{
    return getLayerTemperature(L);
}

template <LayerType L>
Amount<Unit::JOULE_PER_MOLE_CELSIUS> SingleLayerMixture<L>::getLayerHeatCapacity() const
{
    return getLayerHeatCapacity(L);
}

template <LayerType L>
Amount<Unit::JOULE_PER_CELSIUS> SingleLayerMixture<L>::getLayerTotalHeatCapacity() const
{
    return getLayerTotalHeatCapacity(L);
}

template <LayerType L>
Amount<Unit::JOULE_PER_MOLE> SingleLayerMixture<L>::getLayerKineticEnergy() const
{
    return getLayerKineticEnergy(L);
}

template <LayerType L>
Polarity SingleLayerMixture<L>::getLayerPolarity() const
{
    return getLayerPolarity(L);
}

template <LayerType L>
Color SingleLayerMixture<L>::getLayerColor() const
{
    return getLayerColor(L);
}

template <LayerType L>
Amount<Unit::CELSIUS> SingleLayerMixture<L>::getLayerTemperature(const LayerType) const
{
    return layer.temperature;
}

template <LayerType L>
Amount<Unit::JOULE_PER_MOLE_CELSIUS> SingleLayerMixture<L>::getLayerHeatCapacity(const LayerType) const
{
    return layer.getHeatCapacity();
}

template <LayerType L>
Amount<Unit::JOULE_PER_CELSIUS> SingleLayerMixture<L>::getLayerTotalHeatCapacity(const LayerType) const
{
    return layer.getTotalHeatCapacity();
}

template <LayerType L>
Amount<Unit::JOULE_PER_MOLE> SingleLayerMixture<L>::getLayerKineticEnergy(const LayerType) const
{
    return layer.getKineticEnergy();
}

template <LayerType L>
Polarity SingleLayerMixture<L>::getLayerPolarity(const LayerType) const
{
    return layer.getPolarity();
}

template <LayerType L>
Color SingleLayerMixture<L>::getLayerColor(const LayerType) const
{
    return layer.getColor();
}

template <LayerType L>
bool SingleLayerMixture<L>::isEmpty() const
{
    return layer.moles == 0.0_mol;
}

template <LayerType L>
void SingleLayerMixture<L>::copyContentTo(Ref<ContainerBase> destination, const Amount<Unit::LITER> volume) const
{
    // save and use initial volume
    const auto sourceVolume = layer.volume.asStd();

    for (const auto& [_, r] : content) {
        const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
        destination->add(r.mutate(molesToAdd));
    }
}

template <LayerType L>
void SingleLayerMixture<L>::moveContentTo(Ref<ContainerBase> destination, Amount<Unit::LITER> volume)
{
    // save and use initial volume
    const auto sourceVolume = layer.volume.asStd();

    if (volume > sourceVolume)
        volume = sourceVolume;

    for (const auto& [_, r] : content) {
        const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
        destination->add(r.mutate(molesToAdd));
        add(r.mutate(-molesToAdd));
    }
}

template <LayerType L>
SingleLayerMixture<L> SingleLayerMixture<L>::makeCopy() const
{
    return SingleLayerMixture<L>(*this);
}
