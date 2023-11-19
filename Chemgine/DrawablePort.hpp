#pragma once

#include "LabwarePort.hpp"
#include "SFML/Graphics.hpp"

class DrawablePort
{
public:
	const PortType type;
	const float angle;
	float rotationAngle = 0.0f;
	sf::Vector2f position;

	DrawablePort(const LabwarePort& port, const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f)) noexcept;

	void rotate(const float angle);
};