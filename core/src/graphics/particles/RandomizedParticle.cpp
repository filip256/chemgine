#include "graphics/particles/RandomizedParticle.hpp"

#include "graphics/particles/ParticleSystem.hpp"
#include "utils/Math.hpp"

#include <cmath>

RandomizedParticle::RandomizedParticle(const ParticleSystem& system) noexcept :
	system(system)
{
	reset();
	idlespan = system.lifespan * std::abs(getRandom());
}

float_s RandomizedParticle::getRandom()
{
	static std::minstd_rand0 generator(42);
	static std::normal_distribution<float_s> distribution(0.0f, 0.5f);

	return distribution(generator);
}

void RandomizedParticle::reset()
{
	auto rng = getRandom();

	const auto size = system.particleSize + rng * 10.0f;
	shape.setSize(sf::Vector2f(size, size));
	shape.setOrigin(shape.getSize() / 2.0f);
	shape.setPosition(system.origin);
	shape.setFillColor(system.color);

	speed = system.speed - rng * 10.0f;
	lifespan = system.lifespan + rng * 1.5f;
	rotationIncrement = std::clamp(0.1f / rng, 0.5f, 2.0f);

	rng = getRandom();
	direction = system.direction + rng * 20.0f;
	targetDirection = system.targetDirection + rng * 20.0f;

	elapsed = 0.0;
}

void RandomizedParticle::tick(Amount<Unit::SECOND> timespan)
{
	if (idlespan > 0.0f)
	{
		idlespan -= timespan;
		return;
	}

	if (elapsed + timespan > lifespan)
	{
		timespan = elapsed + timespan - lifespan;
		reset();
	}

	elapsed += timespan;
	
	const auto& col = shape.getFillColor();
	const auto fadeMultiplier = ((lifespan - elapsed) / lifespan).asStd();
	shape.setFillColor(sf::Color(col.r, col.g, col.b, static_cast<uint8_t>(system.color.a * fadeMultiplier)));

	const auto distance = speed.to<Unit::NONE>(timespan).asStd();
	const Amount<Unit::RADIAN> radians = direction;
	shape.move(sf::Vector2f(distance * std::cos(radians.asStd()), distance * std::sin(radians.asStd())));

	auto directionIncrement = targetDirection - direction;
	if (directionIncrement > 180.0_o)
		directionIncrement -= 360.0_o;
	direction += directionIncrement * system.directionChangeRate;

	shape.rotate(sf::degrees(rotationIncrement));
}

void RandomizedParticle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (idlespan > 0.0f)
		return;

	target.draw(shape, states);
}
