#pragma once

#include "SFML/Graphics.hpp"

class ShapeFill : public sf::Drawable
{
private:
	sf::Vector2f originPosition = { 0.0f, 0.0f };
	mutable sf::Sprite sprite;

public:
	ShapeFill(const sf::Texture& texture) noexcept;

	void setPosition(const sf::Vector2f& position);
	void move(const sf::Vector2f& offset);
	void setScale(const sf::Vector2f& scale);
	void setRotation(const float angle);
	void setColor(const sf::Color& color) const;
	void setDrawSection(const float start, const float end, const sf::Color& color) const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

	static sf::Texture createFillTexture(
		const sf::Texture& source,
		const uint8_t alphaThreshold = 0);
};
