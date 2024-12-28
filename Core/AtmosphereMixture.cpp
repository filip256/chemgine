#include "AtmosphereMixture.hpp"

AtmosphereMixture::AtmosphereMixture(const AtmosphereMixture& other) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(
		static_cast<const SingleLayerMixture<LayerType::GASEOUS>&>(other).makeCopy())
{}

AtmosphereMixture::AtmosphereMixture(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const ContentInitializer& contentInitializer,
	const Amount<Unit::LITER> maxVolume,
	const Ref<ContainerBase> overflowTarget
) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(temperature, pressure, contentInitializer, maxVolume, overflowTarget)
{}

AtmosphereMixture AtmosphereMixture::createSubatmosphere(const Amount<Unit::LITER> maxVolume)
{
	return AtmosphereMixture(layer.getTemperature(), pressure, content, maxVolume, Ref(*this));
}

AtmosphereMixture AtmosphereMixture::makeCopy() const
{
	return AtmosphereMixture(*this);
}

void AtmosphereMixture::tick(const Amount<Unit::SECOND> timespan)
{
	checkOverflow();
	removeNegligibles();
	consumePotentialEnergy();
}

AtmosphereMixture AtmosphereMixture::createDefaultAtmosphere()
{
	return AtmosphereMixture(1.0_C, 760.0_torr,
		{ { Molecule("N#N"), 78.084_mol }, { Molecule("O=O"), 20.946_mol } },
		10000.0_L, DumpContainer::GlobalDumpContainer);
}
