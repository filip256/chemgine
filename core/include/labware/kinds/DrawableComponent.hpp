#pragma once

#include "graphics/ShapeFill.hpp"
#include "labware/DrawablePort.hpp"
#include "labware/data/DrawableLabwareData.hpp"
#include "labware/kinds/LabwareComponentBase.hpp"

#include <SFML/Graphics.hpp>

class DrawableComponent : public LabwareComponentBase
{
private:
    sf::Sprite                sprite;
    std::vector<DrawablePort> adjustedPorts;

protected:
    DrawableComponent(const LabwareId id, const LabwareType type) noexcept;

public:
    const DrawableLabwareData& getData() const override;

    const sf::Sprite&    getSprite() const override final;
    sf::Sprite&          getSprite() override final;
    sf::Vector2f         getPosition() const override final;
    sf::Vector2f         getAdjustedPosition() const override final;
    void                 setPosition(const sf::Vector2f& position) override;
    void                 move(const sf::Vector2f& offset) override;
    Amount<Unit::DEGREE> getRotation() const override final;
    void                 setRotation(const Amount<Unit::DEGREE> angle) override;
    sf::Vector2f         getOrigin() const override final;
    sf::FloatRect        getBounds() const override final;

    const DrawablePort&              getPort(const uint8_t idx) const override final;
    const std::vector<DrawablePort>& getPorts() const override final;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    bool contains(const sf::Vector2f& point) const override final;
    bool intersects(const LabwareComponentBase& other) const override final;
};
