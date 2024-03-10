#include "Atmosphere.hpp"

Atmosphere::Atmosphere(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const std::vector<std::pair<Molecule, Amount<Unit::MOLE>>>& initContent,
	const Amount<Unit::LITER> maxVolume,
	const Ref<BaseContainer> overflowTarget
) noexcept :
	SingleLayerMixture<LayerType::GASEOUS>(temperature, pressure, initContent, maxVolume, overflowTarget)
{}

void Atmosphere::tick()
{
	checkOverflow();
}