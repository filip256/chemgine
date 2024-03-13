#pragma once

#include "BaseLabwareComponent.hpp"
#include "SFML/Graphics.hpp"
#include "DrawablePort.hpp"

class DrawableComponent : public BaseLabwareComponent
{
private:
	sf::Sprite sprite;
	std::vector<DrawablePort> adjustedPorts;

protected:
	DrawableComponent(const LabwareId id) noexcept;

public:
	const sf::Sprite& getSprite() const override final;
	sf::Sprite& getSprite() override final;
	const sf::Vector2f& getPosition() const override final;
	const sf::Vector2f getAdjustedPosition() const override final;
	void setPosition(const sf::Vector2f& position) override final;
	float getRotation() const override final;
	void setRotation(const float angle) override final;
	const sf::Vector2f& getOrigin() const override final;
	sf::FloatRect getBounds() const override final;

	const DrawablePort& getPort(const uint8_t idx) const override final;
	const std::vector<DrawablePort>& getPorts() const override final;

	void draw(sf::RenderTarget& target) const override final;
	bool contains(const sf::Vector2f& point) const override final;
	bool intersects(const BaseLabwareComponent& other) const override final;
};