#include "Atmosphere.hpp"

Atmosphere::Atmosphere(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const ContentInitializer& contentInitializer,
	const Amount<Unit::LITER> maxVolume,
	const Ref<BaseContainer> overflowTarget
) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(temperature, pressure, contentInitializer, maxVolume, overflowTarget)
{}

void Atmosphere::tick()
{
	checkOverflow();
}
