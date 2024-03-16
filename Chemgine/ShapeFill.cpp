#include "ShapeFill.hpp"
#include "Collision.hpp"
#include "Maths.hpp"

#include <cmath>

ShapeFill::ShapeFill(const sf::Texture& texture) noexcept :
	sprite(texture)
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

void ShapeFill::setDrawSection(const float start, const float end, const sf::Color& color) const
{
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

sf::Texture ShapeFill::createFillTexture(
	const sf::Texture& source,
	const uint8_t alphaThreshold)
{
	const auto& mask = Collision::getTextureMask(source);
	const auto size = source.getSize();

	sf::Image img;
	img.create(size.x, size.y, sf::Color::Transparent);

	for (uint32_t i = 0; i < size.y; ++i)
	{
		uint32_t l = 0;
		while (++l < size.x && mask[i * size.x + l] <= alphaThreshold);

		uint32_t r = size.x;
		while (--r > l && mask[i * size.x + r] <= alphaThreshold);

		for (uint32_t j = l; j < r; ++j)
			img.setPixel(j, i, sf::Color::White);
	}

	for (uint32_t j = 0; j < size.x; ++j)
	{
		uint32_t t = 0;
		while (++t < size.y && mask[t * size.x + j] <= alphaThreshold)
			img.setPixel(j, t, sf::Color::Transparent);

		uint32_t b = size.y;
		while (--b > t && mask[b * size.x + j] <= alphaThreshold)
			img.setPixel(j, b, sf::Color::Transparent);
	}

	sf::Texture temp;
	temp.loadFromImage(img);
	return temp;
}
