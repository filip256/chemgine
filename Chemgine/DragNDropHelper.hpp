#pragma once

#include "SFML/System/Vector2.hpp"

class DragNDropHelper
{
private:
    bool inUse = false;
    sf::Vector2f initialPos = sf::Vector2f(0.0f, 0.0f);

public:
    void start(const sf::Vector2f& point);
    void end();
    void resetOrigin(const sf::Vector2f& point);

    bool isInUse() const;
    sf::Vector2f getDelta(const sf::Vector2f& point) const;
};
