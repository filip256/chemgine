#pragma once

#include <SFML/Window.hpp>

#include <unordered_map>
#include <memory>

class CursorHelper
{
private:
    sf::Window& window;
    
    static std::unordered_map<sf::Cursor::Type, std::unique_ptr<sf::Cursor>> cursors;

public:
    CursorHelper(sf::Window& window) noexcept;
    CursorHelper(sf::Window& window, const sf::Cursor::Type initialType) noexcept;

    bool setType(const sf::Cursor::Type type);
    void setVisibility(const bool visible);
};
