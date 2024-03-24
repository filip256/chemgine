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

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
