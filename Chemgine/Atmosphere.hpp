#pragma once

#include "SingleLayerMixture.hpp"

class Atmosphere : public SingleLayerMixture<LayerType::GASEOUS>
{
private:

public:
	Atmosphere(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const ContentInitializer& contentInitializer,
		const Amount<Unit::LITER> maxVolume,
		const Ref<BaseContainer> overflowTarget
	) noexcept;

	void tick();
};
