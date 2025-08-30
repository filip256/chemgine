#pragma once

#include "data/values/Color.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace utils
{

template <typename Dst, typename Src>
sf::Vector2<Dst> vectorCast(const sf::Vector2<Src>& vec);

const Color&     colorCast(const sf::Color& color);
Color&           colorCast(sf::Color& color);
const sf::Color& colorCast(const Color& color);
sf::Color&       colorCast(Color& color);

};  // namespace utils

template <typename Dst, typename Src>
sf::Vector2<Dst> utils::vectorCast(const sf::Vector2<Src>& vec)
{
    return sf::Vector2<Dst>(static_cast<Dst>(vec.x), static_cast<Dst>(vec.y));
}
