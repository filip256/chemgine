#pragma once

#include "DrawableComponent.hpp"
#include "FlaskData.hpp"
#include "Reactor.hpp"

class Flask : public DrawableComponent
{
private:
	Reactor content;

	Flask(const Flask& other) noexcept;

public:
	Flask(
		const LabwareId id,
		Atmosphere& atmosphere
	) noexcept;

	const FlaskData& getData() const override final;

	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount);
	void tick();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

	Flask* clone() const override final;
};
