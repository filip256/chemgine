#pragma once

#include "ContainerComponent.hpp"
#include "AdaptorData.hpp"
#include "AtmosphereMixture.hpp"

class Adaptor : public ContainerComponent<AtmosphereMixture>
{
public:
	Adaptor(
		const LabwareId id,
		AtmosphereMixture& atmosphere
	) noexcept;

	const AdaptorData& getData() const override final;

	bool tryConnect(LabwareComponentBase& other) override final;
	void disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;
};
