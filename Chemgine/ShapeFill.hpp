#pragma once

#include "ShapeFillTexture.hpp"

#include <utility>

class ShapeFill : public sf::Drawable
{
private:
	sf::Vector2f originPosition = { 0.0f, 0.0f };
	const ShapeFillTexture& texture;
	mutable sf::Sprite sprite;

public:
	ShapeFill(
		const ShapeFillTexture& texture,
		const float scale
	) noexcept;

	void setPosition(const sf::Vector2f& position);
	void move(const sf::Vector2f& offset);
	void setScale(const sf::Vector2f& scale);
	void setRotation(const float angle);
	void setColor(const sf::Color& color) const;
	void setDrawSection(float start, float end, const sf::Color& color) const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
