#pragma once

#include "graphics/particles/ParticleSystem.hpp"

class Vapour : public sf::Drawable
{
private:
	float_s relativeDensity, streamIntensity;
	ParticleSystem particles;

	Amount<Unit::DEGREE> getTargetDirection() const;
	float_s getDirectionChangeRate() const;
	Amount<Unit::PER_SECOND> getSpeed() const;

public:
	Vapour(
		const uint16_t particleCount,
		const sf::Vector2f& origin,
		const sf::Color& color,
		const Amount<Unit::DEGREE> sourceAngle,
		const float_s relativeDensity,
		const float_s streamIntensity
	) noexcept;

	void setColor(const sf::Color& color);
	void setOrigin(const sf::Vector2f& position);
	void moveOrigin(const sf::Vector2f& offset);
	void setRelativeDensity(const float_s relativeDensity);
	void setStreamIntensity(const float_s streamIntensity);

	void tick(const Amount<Unit::SECOND> timespan);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
