#pragma once

#include "ContainerComponent.hpp"
#include "FlaskData.hpp"
#include "Reactor.hpp"

class Flask : public ContainerComponent<Reactor>
{
public:
	Flask(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const FlaskData& getData() const override final;

	bool tryConnect(PortIdentifier& thisPort, PortIdentifier& otherPort) override final;
	void disconnect(PortIdentifier& thisPort, PortIdentifier& otherPort, const Ref<BaseContainer> dump) override final;
};
