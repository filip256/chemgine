#include "helpers/DragNDropHelper.hpp"

void DragNDropHelper::start(const sf::Vector2f& point)
{
    inUse      = true;
    initialPos = point;
}

void DragNDropHelper::end() { inUse = false; }

void DragNDropHelper::resetOrigin(const sf::Vector2f& point) { initialPos = point; }

bool DragNDropHelper::isInUse() const { return inUse; }

sf::Vector2f DragNDropHelper::getDelta(const sf::Vector2f& point) const
{
    return point - initialPos;
}
