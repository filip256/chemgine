#include "ShapeFill.hpp"
#include "Maths.hpp"

#include <cmath>

ShapeFill::ShapeFill(const ShapeFillTexture& texture) noexcept :
	texture(texture),
	sprite(texture.getTexture())
{
	sprite.setOrigin(sprite.getLocalBounds().getSize() / 2.0f);
}

void ShapeFill::setPosition(const sf::Vector2f& position)
{
	sprite.setPosition(position);
	originPosition = position;
}

void ShapeFill::move(const sf::Vector2f& offset)
{
	sprite.move(offset);
	originPosition += offset;
}

void ShapeFill::setScale(const sf::Vector2f& scale)
{
	sprite.setScale(scale);
}

void ShapeFill::setRotation(const float angle)
{
	sprite.setRotation(angle);
}

void ShapeFill::setColor(const sf::Color& color) const
{
	sprite.setColor(color);
}

void ShapeFill::setDrawSection(float start, float end, const sf::Color& color) const
{
	if (texture.hasVolumetricScaling())
	{
		start = texture.getRelativeHeightAt(start);
		end = texture.getRelativeHeightAt(end);
	}

	const auto size = sprite.getTexture()->getSize();
	const auto topCut = size.y * (1.0f - end);
	sprite.setTextureRect(sf::IntRect(0, topCut, size.x, size.y * (end - start)));

	const auto rotation = Maths::toRadians(sprite.getRotation());
	sprite.setPosition(sf::Vector2f(
		originPosition.x - std::sinf(rotation) * topCut,
		originPosition.y + std::cosf(rotation) * topCut
	));

	sprite.setColor(color);
}

void ShapeFill::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(sprite);
}
