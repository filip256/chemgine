#include "graphics/ShapeFill.hpp"

#include <cmath>

ShapeFill::ShapeFill(
	const ShapeFillTexture& texture,
	const float_s scale
) noexcept :
	texture(texture),
	sprite(texture.getTexture())
{
	sprite.setScale(sf::Vector2f(scale, scale));
	sprite.setOrigin(sprite.getGlobalBounds().getSize() / 2.0f);
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

void ShapeFill::setRotation(const Amount<Unit::DEGREE> angle)
{
	sprite.setRotation(angle.asStd());
}

void ShapeFill::setColor(const sf::Color& color) const
{
	sprite.setColor(color);
}

void ShapeFill::setDrawSection(float_s start, float_s end, const sf::Color& color) const
{
	start = std::max(start, 0.0f);
	end = std::min(end, 1.0f);

	if (texture.hasVolumetricScaling())
	{
		start = texture.getRelativeHeightAt(start);
		end = texture.getRelativeHeightAt(end);
	}

	const auto size = sprite.getTexture()->getSize();
	const auto topCut = size.y * (1.0f - end);
	const auto heightCut = std::ceil(size.y * (end - start));
	sprite.setTextureRect(sf::IntRect(0, static_cast<int32_t>(topCut), size.x, static_cast<int32_t>(heightCut)));

	const Amount<Unit::RADIAN> rotation = Amount<Unit::DEGREE>(sprite.getRotation());
	sprite.setPosition(sf::Vector2f(
		originPosition.x - std::sin(rotation.asStd()) * topCut * sprite.getScale().y,
		originPosition.y + std::cos(rotation.asStd()) * topCut * sprite.getScale().y
	));

	sprite.setColor(color);
}

void ShapeFill::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(sprite, states);
}
