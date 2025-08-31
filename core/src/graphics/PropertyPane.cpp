#include "graphics/PropertyPane.hpp"

#include "labware/kinds/BaseContainerComponent.hpp"
#include "mixtures/kinds/MultiLayerMixture.hpp"
#include "mixtures/kinds/SingleLayerMixture.hpp"
#include "utils/Format.hpp"

MixturePropertyPane::LayerPropertyPane::LayerPropertyPane(const sf::Font& font) noexcept :
    title(font, "", 16),
    propertyName(font, "", 16),
    propertyValue(font, "", 16)
{
    title.setStyle(sf::Text::Bold);
    propertyValue.setStyle(sf::Text::Bold);
}

const sf::Vector2f& MixturePropertyPane::LayerPropertyPane::getPosition() const { return position; }

void MixturePropertyPane::LayerPropertyPane::setSubject(const Layer& newSubject)
{
    subject = newSubject;
    title.setString(utils::capitalize(getLayerName(newSubject.getType())) + " Layer:");
}

void MixturePropertyPane::LayerPropertyPane::setPosition(const sf::Vector2f& newPosition)
{
    position = newPosition;
    title.setPosition(newPosition);
    propertyName.setPosition(newPosition + sf::Vector2f(0.0f, 20.0f));
    propertyValue.setPosition(newPosition + sf::Vector2f(110.0f, 20.0f));
}

void MixturePropertyPane::LayerPropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(title, states);

    propertyName.setString("Moles:");
    propertyValue.setString(subject->getMoles().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Mass:");
    propertyValue.setString(subject->getMass().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Volume:");
    propertyValue.setString(subject->getVolume().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Temperature:");
    propertyValue.setString(subject->getTemperature().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Kinetic Energy:");
    propertyValue.setString(subject->getKineticEnergy().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.setPosition(sf::Vector2f(position.x, position.y + 24.0f));
    propertyValue.setPosition(sf::Vector2f(position.x + 110.0f, position.y + 24.0f));
}

MixturePropertyPane::ContentPropertyPane::ContentPropertyPane(const sf::Font& font) noexcept :
    propertyName(font, "", 16),
    propertyValue(font, "", 16)
{
    propertyValue.setStyle(sf::Text::Bold);
}

const sf::Vector2f& MixturePropertyPane::ContentPropertyPane::getPosition() const { return position; }

void MixturePropertyPane::ContentPropertyPane::setSubject(const Layer& newSubject) { subject = newSubject; }

void MixturePropertyPane::ContentPropertyPane::setPosition(const sf::Vector2f& newPosition)
{
    position = newPosition;
    propertyName.setPosition(newPosition);
    propertyValue.setPosition(newPosition + sf::Vector2f(140.0f, 0.0f));
}

void MixturePropertyPane::ContentPropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& m : *subject) {
        propertyName.setString(utils::capitalize(m.molecule.getData().name));
        propertyValue.setString(m.amount.toString(12));
        target.draw(propertyName, states);
        target.draw(propertyValue, states);
        propertyName.move(sf::Vector2f(0.0f, 20.0f));
        propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    }

    propertyName.setPosition(sf::Vector2f(position.x, position.y));
    propertyValue.setPosition(sf::Vector2f(position.x + 110.0f, position.y));
}

MixturePropertyPane::MixturePropertyPane(const sf::Font& font) noexcept :
    title(font, "", 20),
    propertyName(font, "", 16),
    propertyValue(font, "", 16),
    layerPane(font),
    contentPane(font)
{
    title.setStyle(sf::Text::Bold);
    propertyValue.setStyle(sf::Text::Bold);
}

void MixturePropertyPane::setSubject(const LabwareComponentBase& newSubject)
{
    subject = newSubject;
    setPosition(newSubject.getPosition());

#ifndef NDEBUG
    title.setString(newSubject.getData().name + " <" + utils::toHex(&newSubject) + '>');
#else
    title.setString(newSubject.getData().name);
#endif
}

void MixturePropertyPane::setPosition(const sf::Vector2f& newPosition)
{
    position = newPosition;
    title.setPosition(newPosition);
    propertyName.setPosition(newPosition + sf::Vector2f(0.0f, 20.0f));
    propertyValue.setPosition(newPosition + sf::Vector2f(110.0f, 20.0f));
}

void MixturePropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(title, states);

    if (subject->isContainer() == false)
        return;

    const auto& container = static_cast<const BaseContainerComponent*>(&subject.get())->getContent();

    propertyName.setString("Moles:");
    propertyValue.setString(container.getTotalMoles().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Mass:");
    propertyValue.setString(container.getTotalMass().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Total Volume:");
    propertyValue.setString(container.getTotalVolume().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    propertyName.move(sf::Vector2f(0.0f, 20.0f));
    propertyValue.move(sf::Vector2f(0.0f, 20.0f));
    propertyName.setString("Pressure:");
    propertyValue.setString(container.getPressure().toString(6));
    target.draw(propertyName, states);
    target.draw(propertyValue, states);

    if (const auto multiLayerMixture = Ref(container).cast<const MultiLayerMixture>()) {
        propertyName.move(sf::Vector2f(0.0f, 20.0f));
        propertyName.setString("Layers:");
        target.draw(propertyName, states);

        layerPane.setPosition(propertyName.getPosition() + sf::Vector2f(22.0f, 32.0f));
        contentPane.setPosition(propertyName.getPosition() + sf::Vector2f(240.0f, 32.0f));
        for (auto l = multiLayerMixture->getLayersDownBegin(); l != multiLayerMixture->getLayersDownEnd(); ++l) {
            layerPane.setSubject(l->second);
            contentPane.setSubject(l->second);
            target.draw(layerPane, states);
            target.draw(contentPane, states);
            layerPane.setPosition(layerPane.getPosition() + sf::Vector2f(0.0f, 130.0f));
            contentPane.setPosition(contentPane.getPosition() + sf::Vector2f(0.0f, 130.0f));
        }
    }
    else if (const auto atmosphere = Ref(container).cast<const Atmosphere>()) {
        layerPane.setPosition(propertyName.getPosition() + sf::Vector2f(22.0f, 32.0f));
        contentPane.setPosition(propertyName.getPosition() + sf::Vector2f(240.0f, 32.0f));

        layerPane.setSubject(atmosphere->getLayer());
        contentPane.setSubject(atmosphere->getLayer());
        target.draw(layerPane, states);
        target.draw(contentPane, states);
    }

    propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
    propertyValue.setPosition(sf::Vector2f(position.x + 110.0f, position.y + 20.0f));
}
