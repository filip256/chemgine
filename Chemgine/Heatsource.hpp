#pragma once

#include "EquipmentComponent.hpp"
#include "HeatsourceData.hpp"
#include "BaseContainerComponent.hpp"
#include "Atmosphere.hpp"

class Heatsource : public EquipmentComponent
{
private:
	Ref<BaseContainer> target;

public:
	Heatsource(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const HeatsourceData& getData() const override final;

	void setTarget(const Ref<BaseContainer> target);
	void setTarget(BaseContainerComponent& target);

	bool tryConnect(BaseLabwareComponent& other) override final;
	void disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other) override final;

	void tick(const Amount<Unit::SECOND> timespan) override final;
};
