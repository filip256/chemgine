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
	title.setString(Linguistics::capitalize(getLayerName(subject.getType())) + " Layer:");
}

void MixturePropertyPane::LayerPropertyPane::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	title.setPosition(position);
	propertyName.setPosition(sf::Vector2f(position.x, position.y + 16.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 16.0f));
}

void MixturePropertyPane::LayerPropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(title, states);

	propertyName.setString("Moles:");
	propertyValue.setString(subject->getMoles().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Mass:");
	propertyValue.setString(subject->getMass().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Volume:");
	propertyValue.setString(subject->getVolume().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Temperature:");
	propertyValue.setString(subject->getTemperature().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 20.0f));
}



MixturePropertyPane::ContentPropertyPane::ContentPropertyPane(const sf::Font& font) noexcept :
	propertyName("", font, 12),
	propertyValue("", font, 12)
{
	propertyValue.setStyle(sf::Text::Bold);
}

const sf::Vector2f& MixturePropertyPane::ContentPropertyPane::getPosition() const
{
	return position;
}

void MixturePropertyPane::ContentPropertyPane::setSubject(const Layer& subject)
{
	this->subject = subject;
}

void MixturePropertyPane::ContentPropertyPane::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	propertyName.setPosition(sf::Vector2f(position.x, position.y));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y));
}

void MixturePropertyPane::ContentPropertyPane::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const auto& m : *subject)
	{
		propertyName.setString(Linguistics::capitalize(m.molecule.data().name));
		propertyValue.setString(m.amount.toString(12));
		target.draw(propertyName, states);
		target.draw(propertyValue, states);
		propertyName.move(sf::Vector2f(0.0f, 16.0f));
		propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	}

	propertyName.setPosition(sf::Vector2f(position.x, position.y));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y));
}



MixturePropertyPane::MixturePropertyPane(const sf::Font& font) noexcept :
	title("", font, 16),
	propertyName("", font, 12),
	propertyValue("", font, 12),
	layerPane(font),
	contentPane(font)
{
	title.setStyle(sf::Text::Bold);
	propertyValue.setStyle(sf::Text::Bold);
}

void MixturePropertyPane::setSubject(const BaseLabwareComponent& subject)
{
	this->subject = subject;
	title.setString(subject.getData().name);
	setPosition(subject.getPosition());
}

void MixturePropertyPane::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	title.setPosition(position);
	propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 20.0f));
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

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Mass:");
	propertyValue.setString(container.getTotalMass().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Total Volume:");
	propertyValue.setString(container.getTotalVolume().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	propertyName.move(sf::Vector2f(0.0f, 16.0f));
	propertyValue.move(sf::Vector2f(0.0f, 16.0f));
	propertyName.setString("Pressure:");
	propertyValue.setString(container.getPressure().toString(6));
	target.draw(propertyName, states);
	target.draw(propertyValue, states);

	if (const auto contentCast = Ref(container).cast<const MultiLayerMixture>())
	{
		propertyName.move(sf::Vector2f(0.0f, 16.0f));
		propertyName.setString("Layers:");
		target.draw(propertyName, states);

		layerPane.setPosition(propertyName.getPosition() + sf::Vector2f(16.0f, 16.0f));
		contentPane.setPosition(propertyName.getPosition() + sf::Vector2f(220.0f, 32.0f));
		for (auto l = contentCast->getLayersDownBegin(); l != contentCast->getLayersDownEnd(); ++l)
		{
			layerPane.setSubject(l->second);
			contentPane.setSubject(l->second);
			target.draw(layerPane, states);
			target.draw(contentPane, states);
			layerPane.setPosition(layerPane.getPosition() + sf::Vector2f(0.0f, 90.0f));
			contentPane.setPosition(contentPane.getPosition() + sf::Vector2f(0.0f, 90.0f));
		}
	}

	propertyName.setPosition(sf::Vector2f(position.x, position.y + 20.0f));
	propertyValue.setPosition(sf::Vector2f(position.x + 100.0f, position.y + 20.0f));
}
