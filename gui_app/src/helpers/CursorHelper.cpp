#include "helpers/CursorHelper.hpp"

std::unordered_map<sf::Cursor::Type, std::unique_ptr<sf::Cursor>> CursorHelper::cursors;

CursorHelper::CursorHelper(sf::Window& window) noexcept :
    window(window)
{}

CursorHelper::CursorHelper(sf::Window& window, const sf::Cursor::Type initialType) noexcept :
    window(window)
{
    setType(initialType);
}

bool CursorHelper::setType(const sf::Cursor::Type type)
{
    auto c = cursors.find(type);
    if (c == cursors.end()) {
        auto newC = sf::Cursor::createFromSystem(type);
        if (not newC)
            return false;

        c = cursors.emplace(type, std::make_unique<sf::Cursor>(std::move(*newC))).first;
    }

    window.setMouseCursor(*c->second);
    return true;
}

void CursorHelper::setVisibility(const bool visible) { window.setMouseCursorVisible(visible); }
