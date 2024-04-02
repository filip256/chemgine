#include "ColorCast.hpp"

const Color& colorCast(const sf::Color& color)
{
    return reinterpret_cast<const Color&>(color);
}

Color& colorCast(sf::Color& color)
{
    return reinterpret_cast<Color&>(color);
}

const sf::Color& colorCast(const Color& color)
{
    return reinterpret_cast<const sf::Color&>(color);
}

sf::Color& colorCast(Color& color)
{
    return reinterpret_cast<sf::Color&>(color);
}
