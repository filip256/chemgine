#include "CursorHelper.hpp"

std::unordered_map<sf::Cursor::Type, std::unique_ptr<sf::Cursor>> CursorHelper::cursors;

CursorHelper::CursorHelper(sf::Window& window) noexcept:
    window(window)
{}

CursorHelper::CursorHelper(sf::Window& window, const sf::Cursor::Type initialType) noexcept:
    window(window)
{
    setType(initialType);
}

bool CursorHelper::setType(const sf::Cursor::Type type)
{
    auto c = cursors.find(type);
    if (c == cursors.end())
    {
        auto newC = new sf::Cursor();
        if (newC->loadFromSystem(type) == false)
        {
            delete newC;
            return false;
        }

        c = cursors.emplace(std::make_pair(type, newC)).first;
    }

    window.setMouseCursor(*c->second);
    return true;
}

void CursorHelper::setVisibility(const bool visible)
{
    window.setMouseCursorVisible(visible);
}
