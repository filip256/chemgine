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
	const Ref<BaseContainer> overflowTarget
) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(temperature, pressure, contentInitializer, maxVolume, overflowTarget)
{}

Atmosphere Atmosphere::createSubatmosphere(const Amount<Unit::LITER> maxVolume)
{
	return Atmosphere(layer.getTemperature(), pressure, content, maxVolume, Ref(*this));
}

Atmosphere Atmosphere::makeCopy() const
{
	return Atmosphere(*this);
}

void Atmosphere::tick()
{
	checkOverflow();
}
