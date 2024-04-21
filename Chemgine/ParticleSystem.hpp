#pragma once

#include "RandomizedParticle.hpp"

class ParticleSystem : public sf::Drawable
{
private:
	Amount<Unit::DEGREE> direction, targetDirection;
	float directionChangeRate;
	float particleSize;
	sf::Color color;
	Amount<Unit::PER_SECOND> speed;
	Amount<Unit::SECOND> lifespan;
	sf::Vector2f origin;

	std::vector<RandomizedParticle> particles;

public:
	ParticleSystem(
		const uint16_t particleCount,
		const sf::Vector2f& origin,
		const sf::Color& color,
		const float particleSize,
		const Amount<Unit::DEGREE> direction,
		const Amount<Unit::DEGREE> targetDirection,
		const float directionChangeRate,
		const Amount<Unit::PER_SECOND> speed,
		const Amount<Unit::SECOND> lifespan
	) noexcept;
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem(ParticleSystem&&) = default;

	void setDirection(const Amount<Unit::DEGREE> direction);
	void setTargetDirection(const Amount<Unit::DEGREE> direction);
	void setDirectionChangeRate(const float rate);
	void setParticleSize(const float size);
	void setColor(const sf::Color& color);
	void setSpeed(const Amount<Unit::PER_SECOND> speed);
	void setLifespan(const Amount<Unit::SECOND> lifespan);
	void setOrigin(const sf::Vector2f& position);
	void moveOrigin(const sf::Vector2f& offset);

	void tick(const Amount<Unit::SECOND> timespan);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

	friend class RandomizedParticle;
};
