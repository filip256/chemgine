#pragma once

#include "BaseLabwareComponent.hpp"
#include "DrawableLabwareData.hpp"
#include "DrawablePort.hpp"
#include "ShapeFill.hpp"
#include "SFML/Graphics.hpp"

class DrawableComponent : public BaseLabwareComponent
{
private:
	sf::Sprite sprite;
	std::vector<DrawablePort> adjustedPorts;

protected:
	DrawableComponent(
		const LabwareId id,
		const LabwareType type
	) noexcept;

public:
	const DrawableLabwareData& getData() const override;

	const sf::Sprite& getSprite() const override final;
	sf::Sprite& getSprite() override final;
	const sf::Vector2f& getPosition() const override final;
	const sf::Vector2f getAdjustedPosition() const override final;
	void setPosition(const sf::Vector2f& position) override;
	void move(const sf::Vector2f& offset) override;
	Amount<Unit::DEGREE> getRotation() const override final;
	void setRotation(const Amount<Unit::DEGREE> angle) override;
	const sf::Vector2f& getOrigin() const override final;
	sf::FloatRect getBounds() const override final;

	const DrawablePort& getPort(const uint8_t idx) const override final;
	const std::vector<DrawablePort>& getPorts() const override final;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	bool contains(const sf::Vector2f& point) const override final;
	bool intersects(const BaseLabwareComponent& other) const override final;
};
