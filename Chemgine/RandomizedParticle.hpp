#pragma once

#include "SFML/Graphics.hpp"
#include "Amount.hpp"

#include <vector>
#include <random>

class ParticleSystem;

class RandomizedParticle : public sf::Drawable
{
private:
	static float_n getRandom();

	Amount<Unit::DEGREE> direction, targetDirection;
	float_n rotationIncrement;
	Amount<Unit::PER_SECOND> speed;
	Amount<Unit::SECOND> lifespan, idlespan;

	Amount<Unit::SECOND> elapsed = 0.0;
	const ParticleSystem& system;
	sf::RectangleShape shape;

public:
	RandomizedParticle(const ParticleSystem& system) noexcept;
	RandomizedParticle(const RandomizedParticle&) = default;

	void reset();

	void tick(Amount<Unit::SECOND> timespan);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
