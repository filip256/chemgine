#pragma once

#include "SFML/Graphics.hpp"
#include "Units.hpp"

#include <vector>
#include <random>

class ParticleSystem;

class RandomizedParticle : public sf::Drawable
{
private:
	static float_s getRandom();

	Quantity<Degree> direction, targetDirection;
	float_s rotationIncrement;
	Quantity<PerSecond> speed;
	Quantity<Second> lifespan, idlespan;

	Quantity<Second> elapsed = 0.0f * _Second;
	const ParticleSystem& system;
	sf::RectangleShape shape;

public:
	RandomizedParticle(const ParticleSystem& system) noexcept;
	RandomizedParticle(const RandomizedParticle&) = default;

	void reset();

	void tick(Quantity<Second> timespan);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
