#include "ParticleSystem.hpp"

ParticleSystem::ParticleSystem(
	const uint16_t particleCount,
	const sf::Vector2f& origin,
	const sf::Color& color,
	const float_s particleSize,
	const Quantity<Degree> direction,
	const Quantity<Degree> targetDirection,
	const float_s directionChangeRate,
	const Quantity<PerSecond> speed,
	const Quantity<Second> lifespan
) noexcept :
	origin(origin),
	color(color),
	particleSize(particleSize),
	direction(direction),
	targetDirection(targetDirection),
	directionChangeRate(directionChangeRate),
	speed(speed),
	lifespan(lifespan)
{
	particles.reserve(particleCount);
	for (uint16_t i = 0; i < particleCount; ++i)
		particles.emplace_back(*this);
}

void ParticleSystem::setDirection(const Quantity<Degree> direction)
{
	this->direction = direction;
}

void ParticleSystem::setTargetDirection(const Quantity<Degree> direction)
{
	targetDirection = direction;
}

void ParticleSystem::setDirectionChangeRate(const float_s rate)
{
	directionChangeRate = rate;
}

void ParticleSystem::setParticleSize(const float_s size)
{
	particleSize = size;
}

void ParticleSystem::setColor(const sf::Color& color)
{
	this->color = color;
}

void ParticleSystem::setSpeed(const Quantity<PerSecond> speed)
{
	this->speed = speed;
}

void ParticleSystem::setLifespan(const Quantity<Second> lifespan)
{
	this->lifespan = lifespan;
}

void ParticleSystem::setOrigin(const sf::Vector2f& position)
{
	origin = position;
}

void ParticleSystem::moveOrigin(const sf::Vector2f& offset)
{
	origin += offset;
}

void ParticleSystem::tick(const Quantity<Second> timespan)
{
	for (uint16_t i = 0; i < particles.size(); ++i)
		particles[i].tick(timespan);
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (uint16_t i = 0; i < particles.size(); ++i)
		target.draw(particles[i], states);
}
