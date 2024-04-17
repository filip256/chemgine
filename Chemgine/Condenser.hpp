#pragma once

#include "ContainerComponent.hpp"
#include "Atmosphere.hpp"
#include "Reactor.hpp"
#include "CondenserData.hpp"

class Condenser : public ContainerComponent<Atmosphere, Reactor>
{
public:
	Condenser(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const CondenserData& getData() const override final;

	bool tryConnect(BaseLabwareComponent& other) override final;
	void disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other) override final;

	void tick(const Amount<Unit::SECOND> timespan) override final;
};
