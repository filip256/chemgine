#pragma once

#include "ContainerComponent.hpp"
#include "FlaskData.hpp"
#include "Reactor.hpp"

class Flask : public ContainerComponent<Reactor>
{
public:
	Flask(
		const LabwareId id,
		AtmosphereMixture& atmosphere
	) noexcept;

	const FlaskData& getData() const override final;

	bool tryConnect(LabwareComponentBase& other) override final;
	void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;
};
