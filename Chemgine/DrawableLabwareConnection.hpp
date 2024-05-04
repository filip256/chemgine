#pragma once

#include "LabwareConnectionData.hpp"
#include "SFML/Graphics.hpp"

class DrawableLabwareConnection
{
private:
	const LabwareConnectionData& data;
	
	sf::Vector2f position;
	Amount<Unit::DEGREE> rotationAngle = 0.0f;

public:
	DrawableLabwareConnection(
		const LabwareConnectionData& data,
		const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f),
		const float scale = 1.0f
	) noexcept;

	PortType getType() const;
	Amount<Unit::DEGREE> getAngle() const;
	const sf::Vector2f& getPosition() const;

	void rotate(const Amount<Unit::DEGREE> angle);
};
