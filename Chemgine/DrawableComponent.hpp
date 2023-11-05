#pragma once

#include "BaseLabwareComponent.hpp"
#include "SFML/Graphics.hpp"

class DrawableComponent : public BaseLabwareComponent
{
private:
	sf::Sprite sprite;

protected:
	DrawableComponent(const LabwareIdType id) noexcept;

public:
	const sf::Sprite& getSprite() const override final;
	sf::Sprite& getSprite() override final;
	void draw(sf::RenderTarget& target) const override final;
	bool contains(const sf::Vector2f& point) const override final;
	bool intersects(const BaseLabwareComponent& other) const override final;
};