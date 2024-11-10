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

	bool tryConnect(LabwareComponentBase& other) override final;
	void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;

	void tick(const Amount<Unit::SECOND> timespan) override final;
};
