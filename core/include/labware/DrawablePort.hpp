#pragma once

#include "labware/LabwarePort.hpp"

#include <SFML/Graphics.hpp>

class DrawablePort
{
public:
    const PortType             type;
    const Amount<Unit::DEGREE> angle;
    Amount<Unit::DEGREE>       rotationAngle = 0.0f;
    sf::Vector2f               position;

    DrawablePort(
        const LabwarePort& port, const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f), const float_s scale = 1.0f) noexcept;

    void rotate(const Amount<Unit::DEGREE> angle);
};
