#include "Vapour.hpp"
#include "Maths.hpp"

Vapour::Vapour(
	const uint16_t particleCount,
	const sf::Vector2f& origin,
	const sf::Color& color,
	const Amount<Unit::DEGREE> sourceAngle,
	const float_s relativeDensity,
	const float_s streamIntensity
) noexcept :
	relativeDensity(relativeDensity),
	streamIntensity(streamIntensity),
	particles(
		particleCount,
		origin,
		color,
		18.0f,
		sourceAngle,
		getTargetDirection(), getDirectionChangeRate(), getSpeed(),
		5.0_s)
{}

Amount<Unit::DEGREE> Vapour::getTargetDirection() const
{
	return relativeDensity < 1.0f ? 270.0_o : 90.0_o;
}

float_s Vapour::getDirectionChangeRate() const
{
	return (std::abs(1.0f - relativeDensity) / streamIntensity) * 0.005f;
}

Amount<Unit::PER_SECOND> Vapour::getSpeed() const
{
	return (std::abs(1.0f - relativeDensity) + streamIntensity) * 50.0f;
}

void Vapour::setColor(const sf::Color& color)
{
	particles.setColor(color);
}

void Vapour::setOrigin(const sf::Vector2f& position)
{
	particles.setOrigin(position);
}

void Vapour::moveOrigin(const sf::Vector2f& offset)
{
	particles.moveOrigin(offset);
}

void Vapour::setRelativeDensity(const float_s relativeDensity)
{
	this->relativeDensity = relativeDensity;
	particles.setTargetDirection(getTargetDirection());
	particles.setDirectionChangeRate(getDirectionChangeRate());
	particles.setSpeed(getSpeed());
}

void Vapour::setStreamIntensity(const float_s streamIntensity)
{
	this->streamIntensity = streamIntensity;
	particles.setDirectionChangeRate(getDirectionChangeRate());
	particles.setSpeed(getSpeed());
}

void Vapour::tick(const Amount<Unit::SECOND> timespan)
{
	particles.tick(timespan);
}

void Vapour::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(particles, states);
}
