#pragma once

#include "EquipmentComponent.hpp"
#include "HeatsourceData.hpp"
#include "Mixture.hpp"
#include "BaseContainerComponent.hpp"

class Heatsource : public EquipmentComponent
{
private:
	Ref<Mixture> target = nullRef;

public:
	Heatsource(const LabwareId id) noexcept;

	const HeatsourceData& getData() const override final;

	void setTarget(const Ref<Mixture> target);
	void setTarget(BaseContainerComponent& target);

	void tick(const Amount<Unit::SECOND> timespan) override final;
};
