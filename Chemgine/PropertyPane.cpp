#include "PropertyPane.hpp"
#include "BaseContainerComponent.hpp"
#include "SingleLayerMixture.hpp"
#include "MultiLayerMixture.hpp"
#include "Linguistics.hpp"

MixturePropertyPane::LayerPropertyPane::LayerPropertyPane(const sf::Font& font) noexcept :
	title("", font, 12),
	propertyName("", font, 12),
	propertyValue("", font, 12)
{
	title.setStyle(sf::Text::Bold);
	propertyValue.setStyle(sf::Text::Bold);
}

const sf::Vector2f& MixturePropertyPane::LayerPropertyPane::getPosition() const
{
	return position;
}

void MixturePropertyPane::LayerPropertyPane::setSubject(const Layer& subject)
{
	this->subject = subject;
	this->title.setString(Linguistics::capitalize(getLayerName(subject.getType())) + " Layer:");
}

void MixturePropertyPane::LayerPropertyPane::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	this->title.setPosition(position);
	propertyName.setPosition(sf::Vector2f(position.x, position.y + 16.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 16.0f));
}

void MixturePropertyPane::LayerPropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(title);

	propertyName.setString("Moles:");
	propertyValue.setString(subject->getMoles().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Mass:");
	propertyValue.setString(subject->getMass().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Volume:");
	propertyValue.setString(subject->getVolume().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Temperature:");
	propertyValue.setString(subject->getTemperature().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 20.0f));
}



MixturePropertyPane::MixturePropertyPane(const sf::Font& font) noexcept :
	title("", font, 16),
	propertyName("", font, 12),
	propertyValue("", font, 12),
	layerPane(font)
{
	title.setStyle(sf::Text::Bold);
	propertyValue.setStyle(sf::Text::Bold);
}

void MixturePropertyPane::setSubject(const BaseLabwareComponent& subject)
{
	this->subject = subject;
	this->title.setString(subject.getData().name);
	setPosition(subject.getPosition());
}

void MixturePropertyPane::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	this->title.setPosition(position);
	propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 20.0f));
}

void MixturePropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(title);

	if (subject->isContainerType() == false)
		return;

	const auto& container = static_cast<const BaseContainerComponent*>(&subject.get())->getContent();

	propertyName.setString("Moles:");
	propertyValue.setString(container.getTotalMoles().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Mass:");
	propertyValue.setString(container.getTotalMass().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Total Volume:");
	propertyValue.setString(container.getTotalVolume().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Pressure:");
	propertyValue.setString(container.getPressure().toString(6));
	target.draw(propertyName);
	target.draw(propertyValue);

	if (const auto contentCast = Ref(container).as<const MultiLayerMixture>())
	{
		propertyName.move(sf::Vector2f(0.0f, 16.0f));
		propertyName.setString("Layers:");
		target.draw(propertyName);

		layerPane.setPosition(propertyName.getPosition() + sf::Vector2f(16.0f, 16.0f));

		for (auto l = contentCast->getLayersDownBegin(); l != contentCast->getLayersDownEnd(); ++l)
		{
			layerPane.setSubject(l->second);
			target.draw(layerPane);
			layerPane.setPosition(layerPane.getPosition() + sf::Vector2f(0.0f, 90.0f));
		}
	}

	propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 20.0f));
}
