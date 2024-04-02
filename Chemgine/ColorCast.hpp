#pragma once

#include "Color.hpp"
#include "SFML/Graphics/Color.hpp"

const Color& colorCast(const sf::Color& color);
Color& colorCast(sf::Color& color);

const sf::Color& colorCast(const Color& color);
sf::Color& colorCast(Color& color);
