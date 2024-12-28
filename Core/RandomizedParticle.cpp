#include "RandomizedParticle.hpp"
#include "ParticleSystem.hpp"
#include "Maths.hpp"

#include <cmath>

RandomizedParticle::RandomizedParticle(const ParticleSystem& system) noexcept :
	system(system)
{
	reset();
	idlespan = system.lifespan * abs(getRandom());
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

	const auto size = system.particleSize + rng * 10;
	shape.setSize(sf::Vector2f(size, size));
	shape.setOrigin(shape.getSize() / 2.0f);
	shape.setPosition(system.origin);
	shape.setFillColor(system.color);

	speed = system.speed - rng * 10.0f * _PerSecond;
	lifespan = system.lifespan + rng * 1.5f * _Second;
	rotationIncrement = Maths::clamp(0.1 / rng, 0.5, 2.0);

	rng = getRandom();
	direction = system.direction + rng * 20 * _Degree;
	targetDirection = system.targetDirection + rng * 20 * _Degree;

	elapsed = 0.0f * _Second;
}

void RandomizedParticle::tick(Quantity<Second> timespan)
{
	if (idlespan > 0.0f * _Second)
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
	shape.setFillColor(sf::Color(col.r, col.g, col.b, system.color.a * ((lifespan - elapsed) / lifespan).value()));

	const auto distance = speed * timespan;
	const auto radians = direction.to<Radian>().value();
	shape.move(sf::Vector2f(distance * std::cosf(radians), distance * std::sinf(radians)));

	auto directionIncrement = targetDirection - direction;
	if (directionIncrement > 180.0f * _Degree)
		directionIncrement -= 360.0f * _Degree;
	direction += directionIncrement * system.directionChangeRate;

	shape.rotate(rotationIncrement);
}

void RandomizedParticle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (idlespan > 0.0f * _Second)
		return;

	target.draw(shape, states);
}
