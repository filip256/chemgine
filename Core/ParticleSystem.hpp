#pragma once

#include "RandomizedParticle.hpp"

class ParticleSystem : public sf::Drawable
{
private:
	Quantity<Degree> direction, targetDirection;
	float_s directionChangeRate;
	float_s particleSize;
	sf::Color color;
	Quantity<PerSecond> speed;
	Quantity<Second> lifespan;
	sf::Vector2f origin;

	std::vector<RandomizedParticle> particles;

public:
	ParticleSystem(
		const uint16_t particleCount,
		const sf::Vector2f& origin,
		const sf::Color& color,
		const float_s particleSize,
		const Quantity<Degree> direction,
		const Quantity<Degree> targetDirection,
		const float_s directionChangeRate,
		const Quantity<PerSecond> speed,
		const Quantity<Second> lifespan
	) noexcept;
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem(ParticleSystem&&) = default;

	void setDirection(const Quantity<Degree> direction);
	void setTargetDirection(const Quantity<Degree> direction);
	void setDirectionChangeRate(const float_s rate);
	void setParticleSize(const float_s size);
	void setColor(const sf::Color& color);
	void setSpeed(const Quantity<PerSecond> speed);
	void setLifespan(const Quantity<Second> lifespan);
	void setOrigin(const sf::Vector2f& position);
	void moveOrigin(const sf::Vector2f& offset);

	void tick(const Quantity<Second> timespan);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

	friend class RandomizedParticle;
};
