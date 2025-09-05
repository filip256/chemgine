#include "utils/SFML.hpp"

const Color& utils::colorCast(const sf::Color& color) { return reinterpret_cast<const Color&>(color); }

Color& utils::colorCast(sf::Color& color) { return reinterpret_cast<Color&>(color); }

const sf::Color& utils::colorCast(const Color& color) { return reinterpret_cast<const sf::Color&>(color); }

sf::Color& utils::colorCast(Color& color) { return reinterpret_cast<sf::Color&>(color); }
