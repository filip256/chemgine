#include "labware/kinds/DrawableComponent.hpp"

#include "graphics/Collision.hpp"
#include "labware/data/DrawableLabwareData.hpp"

DrawableComponent::DrawableComponent(const LabwareId id, const LabwareType type) noexcept :
    LabwareComponentBase(id, type),
    sprite(getData().texture)
{
    const auto txScale = getData().textureScale;
    sprite.setScale(sf::Vector2f(txScale, txScale));
    sprite.setOrigin(sprite.getGlobalBounds().size / 2.0f);

    adjustedPorts.reserve(data.ports.size());
    for (uint8_t i = 0; i < data.ports.size(); ++i)
        adjustedPorts.emplace_back(data.ports[i], sprite.getOrigin(), txScale);
}

const DrawableLabwareData& DrawableComponent::getData() const
{
    return static_cast<const DrawableLabwareData&>(data);
}

const sf::Sprite& DrawableComponent::getSprite() const { return sprite; }

sf::Sprite& DrawableComponent::getSprite() { return sprite; }

sf::Vector2f DrawableComponent::getPosition() const { return sprite.getPosition(); }

sf::Vector2f DrawableComponent::getAdjustedPosition() const
{
    return sprite.getPosition() - sprite.getOrigin();
}

void DrawableComponent::setPosition(const sf::Vector2f& position) { sprite.setPosition(position); }

void DrawableComponent::move(const sf::Vector2f& offset) { sprite.move(offset); }

Amount<Unit::DEGREE> DrawableComponent::getRotation() const
{
    return sprite.getRotation().asDegrees();
}

void DrawableComponent::setRotation(const Amount<Unit::DEGREE> angle)
{
    for (uint8_t i = 0; i < adjustedPorts.size(); ++i) adjustedPorts[i].rotate(angle);
    sprite.setRotation(sf::degrees(angle.asStd()));
}

sf::Vector2f DrawableComponent::getOrigin() const { return sprite.getOrigin(); }

sf::FloatRect DrawableComponent::getBounds() const { return sprite.getGlobalBounds(); }

const DrawablePort& DrawableComponent::getPort(const uint8_t idx) const
{
    return adjustedPorts[idx];
}

const std::vector<DrawablePort>& DrawableComponent::getPorts() const { return adjustedPorts; }

void DrawableComponent::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#ifndef NDEBUG
    // sf::RectangleShape bBox(sprite.getGlobalBounds().getSize());
    // bBox.setPosition(sprite.getGlobalBounds().getPosition());
    // bBox.setFillColor(sf::Color(255, 255, 255, 25));
    // target.draw(bBox, states);
#endif

    target.draw(sprite, states);

#ifndef NDEBUG
    sf::CircleShape port(2.0f, 16);
    port.setOrigin(sf::Vector2f(1.0f, 1.0f));
    port.setFillColor(sf::Color::Blue);
    // port.setOutlineColor(sf::Color(0, 0, 255, 50));
    // port.setOutlineThickness(28.0f);
    for (uint8_t i = 0; i < adjustedPorts.size(); ++i) {
        port.setPosition(sprite.getPosition() + adjustedPorts[i].position);
        target.draw(port, states);
    }
#endif
}

bool DrawableComponent::contains(const sf::Vector2f& point) const
{
    if (sprite.getGlobalBounds().contains(point) == false)
        return false;

    return Collision::singlePixelTest(sprite, point);
}

bool DrawableComponent::intersects(const LabwareComponentBase& other) const
{
    if (not this->sprite.getGlobalBounds().findIntersection(other.getSprite().getGlobalBounds()))
        return false;

    return Collision::pixelPerfectTest(this->sprite, other.getSprite());
}
