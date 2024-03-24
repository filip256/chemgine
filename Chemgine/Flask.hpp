#pragma once

#include "ContainerComponent.hpp"
#include "FlaskData.hpp"
#include "Reactor.hpp"

class Flask : public ContainerComponent<Reactor>
{
private:
	Flask(const Flask& other) = default;

public:
	Flask(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const FlaskData& getData() const override final;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
