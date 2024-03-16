#include "ColorCast.hpp"

Color colorCast(const sf::Color& color)
{
    return Color(color.r, color.g, color.b, color.a);
}

sf::Color colorCast(const Color& color)
{
    return sf::Color(color.r, color.g, color.b, color.a);
}
