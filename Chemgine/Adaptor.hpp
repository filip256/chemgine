#pragma once

#include "ContainerComponent.hpp"
#include "AdaptorData.hpp"
#include "Atmosphere.hpp"

class Adaptor : public ContainerComponent<Atmosphere>
{
private:
	Adaptor(const Adaptor& other) = default;

public:
	Adaptor(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const AdaptorData& getData() const override final;

	bool tryConnect(BaseLabwareComponent& other) override final;
	void disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other) override final;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
