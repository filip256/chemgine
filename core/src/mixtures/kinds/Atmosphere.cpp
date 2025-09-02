#include "mixtures/kinds/Atmosphere.hpp"

Atmosphere::Atmosphere(const Atmosphere& other) noexcept :
    SingleLayerMixture<LayerType::GASEOUS>(static_cast<const SingleLayerMixture<LayerType::GASEOUS>&>(other).makeCopy())
{}

Atmosphere::Atmosphere(
    const Amount<Unit::CELSIUS> temperature,
    const Amount<Unit::TORR>    pressure,
    const ContentInitializer&   contentInitializer,
    const Amount<Unit::LITER>   maxVolume,
    const Ref<ContainerBase>    overflowTarget) noexcept :
    SingleLayerMixture<LayerType::GASEOUS>(temperature, pressure, contentInitializer, maxVolume, overflowTarget)
{}

Atmosphere Atmosphere::createSubatmosphere(const Amount<Unit::LITER> subMaxVolume)
{
    return Atmosphere(layer.getTemperature(), pressure, content, subMaxVolume, Ref(*this));
}

Atmosphere Atmosphere::makeCopy() const { return Atmosphere(*this); }

void Atmosphere::tick(const Amount<Unit::SECOND>)
{
    checkOverflow();
    removeNegligibles();
    consumePotentialEnergy();
}

std::unique_ptr<Atmosphere> Atmosphere::createDefaultAtmosphere()
{
    return std::make_unique<Atmosphere>(
        1.0_C,
        760.0_torr,
        ContentInitializer{
            {Molecule("N#N"), 78.084_mol},
            {Molecule("O=O"), 20.946_mol}
    },
        10000.0_L,
        DumpContainer::GlobalDumpContainer);
}
