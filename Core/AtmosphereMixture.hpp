#pragma once

#include "SingleLayerMixture.hpp"

class AtmosphereMixture : public SingleLayerMixture<LayerType::GASEOUS>
{
private:
	AtmosphereMixture(const AtmosphereMixture& other) noexcept;

public:
	AtmosphereMixture(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const ContentInitializer& contentInitializer,
		const Amount<Unit::LITER> maxVolume,
		const Ref<ContainerBase> overflowTarget
	) noexcept;
	AtmosphereMixture(AtmosphereMixture&&) = default;

	void tick(const Amount<Unit::SECOND> timespan);

	AtmosphereMixture createSubatmosphere(const Amount<Unit::LITER> maxVolume);

	AtmosphereMixture makeCopy() const;

	static AtmosphereMixture createDefaultAtmosphere();
};
