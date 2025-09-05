#pragma once

#include "data/values/Amount.hpp"
#include "graphics/ShapeFillTexture.hpp"

#include <utility>

class ShapeFill : public sf::Drawable
{
private:
    sf::Vector2f            originPosition = {0.0f, 0.0f};
    const ShapeFillTexture& texture;
    mutable sf::Sprite      sprite;

public:
    ShapeFill(const ShapeFillTexture& texture, const float_s scale) noexcept;

    void setPosition(const sf::Vector2f& position);
    void move(const sf::Vector2f& offset);
    void setScale(const sf::Vector2f& scale);
    void setRotation(const Amount<Unit::DEGREE> angle);
    void setColor(const sf::Color& color) const;
    void setDrawSection(float_s start, float_s end, const sf::Color& color) const;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
