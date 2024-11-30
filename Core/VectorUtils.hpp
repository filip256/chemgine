#pragma once

#include "SFML/Graphics.hpp"

class Utils
{
public:
    template <typename O, typename I>
    static sf::Vector2<O> cast(const sf::Vector2<I>& vec);
};

template <typename O, typename I>
sf::Vector2<O> Utils::cast(const sf::Vector2<I>& vec)
{
    return sf::Vector2<O>(static_cast<O>(vec.x), static_cast<O>(vec.y));
}
