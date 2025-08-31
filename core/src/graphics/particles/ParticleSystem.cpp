#include "graphics/particles/ParticleSystem.hpp"

ParticleSystem::ParticleSystem(
    const uint16_t                 particleCount,
    const sf::Vector2f&            origin,
    const sf::Color&               color,
    const float_s                  particleSize,
    const Amount<Unit::DEGREE>     direction,
    const Amount<Unit::DEGREE>     targetDirection,
    const float_s                  directionChangeRate,
    const Amount<Unit::PER_SECOND> speed,
    const Amount<Unit::SECOND>     lifespan) noexcept :
    direction(direction),
    targetDirection(targetDirection),
    directionChangeRate(directionChangeRate),
    particleSize(particleSize),
    color(color),
    speed(speed),
    lifespan(lifespan),
    origin(origin)
{
    particles.reserve(particleCount);
    for (uint16_t i = 0; i < particleCount; ++i)
        particles.emplace_back(*this);
}

void ParticleSystem::setDirection(const Amount<Unit::DEGREE> newDirection) { direction = newDirection; }

void ParticleSystem::setTargetDirection(const Amount<Unit::DEGREE> newDirection) { targetDirection = newDirection; }

void ParticleSystem::setDirectionChangeRate(const float_s rate) { directionChangeRate = rate; }

void ParticleSystem::setParticleSize(const float_s size) { particleSize = size; }

void ParticleSystem::setColor(const sf::Color& newColor) { color = newColor; }

void ParticleSystem::setSpeed(const Amount<Unit::PER_SECOND> newSpeed) { speed = newSpeed; }

void ParticleSystem::setLifespan(const Amount<Unit::SECOND> newLifespan) { lifespan = newLifespan; }

void ParticleSystem::setOrigin(const sf::Vector2f& position) { origin = position; }

void ParticleSystem::moveOrigin(const sf::Vector2f& offset) { origin += offset; }

void ParticleSystem::tick(const Amount<Unit::SECOND> timespan)
{
    for (uint16_t i = 0; i < particles.size(); ++i)
        particles[i].tick(timespan);
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (uint16_t i = 0; i < particles.size(); ++i)
        target.draw(particles[i], states);
}
