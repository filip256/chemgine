#include "Atmosphere.hpp"

Atmosphere::Atmosphere(const Atmosphere& other) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(
		static_cast<const SingleLayerMixture<LayerType::GASEOUS>&>(other).makeCopy())
{}

Atmosphere::Atmosphere(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const ContentInitializer& contentInitializer,
	const Amount<Unit::LITER> maxVolume,
	std::vector<Ref<BaseContainer>>&& overflowTargets
) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(temperature, pressure, contentInitializer, maxVolume, std::move(overflowTargets))
{}

Atmosphere::Atmosphere(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const ContentInitializer& contentInitializer,
	const Amount<Unit::LITER> maxVolume,
	const Ref<BaseContainer> overflowTarget,
	const uint8_t overflowTargetCount
) noexcept :
	Atmosphere(
		temperature, pressure, contentInitializer, maxVolume,
		std::vector<Ref<BaseContainer>>(overflowTargetCount, overflowTarget))
{}

Atmosphere Atmosphere::createSubatmosphere(const Amount<Unit::LITER> maxVolume)
{
	return Atmosphere(layer.getTemperature(), pressure, content, maxVolume, Ref(*this), 1);
}

Atmosphere Atmosphere::makeCopy() const
{
	return Atmosphere(*this);
}

void Atmosphere::tick(const Amount<Unit::SECOND> timespan)
{
	checkOverflow();
}

Atmosphere Atmosphere::createDefaultAtmosphere()
{
	return Atmosphere(1.0_C, 760.0_torr,
		{ { Molecule("N#N"), 78.084_mol }, { Molecule("O=O"), 20.946_mol } },
		10000.0_L, DumpContainer::GlobalDumpContainer, 1);
}
