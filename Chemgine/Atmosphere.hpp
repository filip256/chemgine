#pragma once

#include "SingleLayerMixture.hpp"

class Atmosphere : public SingleLayerMixture<LayerType::GASEOUS>
{
private:
	Atmosphere(const Atmosphere& other) noexcept;

public:
	Atmosphere(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const ContentInitializer& contentInitializer,
		const Amount<Unit::LITER> maxVolume,
		const Ref<ContainerBase> overflowTarget
	) noexcept;
	Atmosphere(Atmosphere&&) = default;

	void tick(const Amount<Unit::SECOND> timespan);

	Atmosphere createSubatmosphere(const Amount<Unit::LITER> maxVolume);

	Atmosphere makeCopy() const;

	static Atmosphere createDefaultAtmosphere();
};
