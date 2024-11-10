#pragma once

#include "EquipmentComponent.hpp"
#include "HeatsourceData.hpp"
#include "BaseContainerComponent.hpp"
#include "Atmosphere.hpp"

class Heatsource : public EquipmentComponent
{
private:
	Ref<ContainerBase> target;

public:
	Heatsource(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const HeatsourceData& getData() const override final;

	void setTarget(const Ref<ContainerBase> target);
	void setTarget(BaseContainerComponent& target);

	bool tryConnect(LabwareComponentBase& other) override final;
	void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;

	void tick(const Amount<Unit::SECOND> timespan) override final;
};
