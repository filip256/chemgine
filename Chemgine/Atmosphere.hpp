#pragma once

#include "SingleLayerMixture.hpp"

class Atmosphere : public SingleLayerMixture<LayerType::GASEOUS>
{
private:

public:
	Atmosphere(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const std::vector<std::pair<Molecule, Amount<Unit::MOLE>>>& initContent,
		const Amount<Unit::LITER> maxVolume,
		const Ref<BaseContainer> overflowTarget
	) noexcept;

	void tick();
};