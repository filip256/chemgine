#pragma once

#include "Color.hpp"
#include "SFML/Graphics/Color.hpp"

Color colorCast(const sf::Color& color);

sf::Color colorCast(const Color& color);
