#pragma once

#include "DrawableComponent.hpp"
#include "AdaptorData.hpp"
#include "Atmosphere.hpp"

class Adaptor : public DrawableComponent
{
private:
	Atmosphere content;

	Adaptor(const Adaptor& other) noexcept;

public:
	Adaptor(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const AdaptorData& getData() const override final;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
