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

	bool tryConnect(BaseLabwareComponent& other) override final;
	void disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other) override final;
};
