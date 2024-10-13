#pragma once

#include "ParticleSystem.hpp"

class Vapour : public sf::Drawable
{
private:
	float_n relativeDensity, streamIntensity;
	ParticleSystem particles;

	Amount<Unit::DEGREE> getTargetDirection() const;
	float_n getDirectionChangeRate() const;
	Amount<Unit::PER_SECOND> getSpeed() const;

public:
	Vapour(
		const uint16_t particleCount,
		const sf::Vector2f& origin,
		const sf::Color& color,
		const Amount<Unit::DEGREE> sourceAngle,
		const float_n relativeDensity,
		const float_n streamIntensity
	) noexcept;

	void setColor(const sf::Color& color);
	void setOrigin(const sf::Vector2f& position);
	void moveOrigin(const sf::Vector2f& offset);
	void setRelativeDensity(const float_n relativeDensity);
	void setStreamIntensity(const float_n streamIntensity);

	void tick(const Amount<Unit::SECOND> timespan);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
