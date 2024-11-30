#include "ColorCast.hpp"

const Color& Utils::colorCast(const sf::Color& color)
{
    return reinterpret_cast<const Color&>(color);
}

Color& Utils::colorCast(sf::Color& color)
{
    return reinterpret_cast<Color&>(color);
}

const sf::Color& Utils::colorCast(const Color& color)
{
    return reinterpret_cast<const sf::Color&>(color);
}

sf::Color& Utils::colorCast(Color& color)
{
    return reinterpret_cast<sf::Color&>(color);
}
