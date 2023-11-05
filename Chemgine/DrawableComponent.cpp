#include "DrawableComponent.hpp"
#include "Collision.hpp"
#include "DrawableLabwareData.hpp"

DrawableComponent::DrawableComponent(const LabwareIdType id) noexcept :
	BaseLabwareComponent(id),
	sprite(static_cast<const DrawableLabwareData&>(data).texture)
{}

const sf::Sprite& DrawableComponent::getSprite() const
{
	return sprite;
}

sf::Sprite& DrawableComponent::getSprite()
{
	return sprite;
}

void DrawableComponent::draw(sf::RenderTarget& target) const
{
#ifndef NDEBUG
	sf::RectangleShape bBox(sprite.getGlobalBounds().getSize());
	bBox.setPosition(sprite.getGlobalBounds().getPosition());
	bBox.setFillColor(sf::Color(255, 255, 255, 50));
	target.draw(bBox);
#endif

	target.draw(sprite);
}

bool DrawableComponent::contains(const sf::Vector2f& point) const
{
	if (sprite.getGlobalBounds().contains(point) == false)
		return false;

	return Collision::singlePixelTest(sprite, point);
}

bool DrawableComponent::intersects(const BaseLabwareComponent& other) const
{
	if (this->sprite.getGlobalBounds().intersects(other.getSprite().getGlobalBounds()) == false)
		return false;

	return Collision::pixelPerfectTest(this->sprite, other.getSprite());
}