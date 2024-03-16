#pragma once

#include "BaseLabwareComponent.hpp"
#include "SFML/Graphics.hpp"
#include "DrawablePort.hpp"
#include "ShapeFill.hpp"

class DrawableComponent : public BaseLabwareComponent
{
private:
	sf::Sprite sprite;
	std::vector<DrawablePort> adjustedPorts;

protected:
	ShapeFill fill;

	DrawableComponent(const LabwareId id) noexcept;

public:
	const sf::Sprite& getSprite() const override final;
	sf::Sprite& getSprite() override final;
	const sf::Vector2f& getPosition() const override final;
	const sf::Vector2f getAdjustedPosition() const override final;
	void setPosition(const sf::Vector2f& position) override final;
	void move(const sf::Vector2f& offset) override final;
	float getRotation() const override final;
	void setRotation(const float angle) override final;
	const sf::Vector2f& getOrigin() const override final;
	sf::FloatRect getBounds() const override final;

	const DrawablePort& getPort(const uint8_t idx) const override final;
	const std::vector<DrawablePort>& getPorts() const override final;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	bool contains(const sf::Vector2f& point) const override final;
	bool intersects(const BaseLabwareComponent& other) const override final;
};