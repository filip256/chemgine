#pragma once

#include "ContainerComponent.hpp"
#include "AdaptorData.hpp"
#include "Atmosphere.hpp"

class Adaptor : public ContainerComponent<Atmosphere>
{
public:
	Adaptor(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const AdaptorData& getData() const override final;

	bool tryConnect(PortIdentifier& thisPort, PortIdentifier& otherPort) override final;
	void disconnect(PortIdentifier& thisPort, PortIdentifier& otherPort, const Ref<BaseContainer> dump) override final;
};
