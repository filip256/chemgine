#pragma once

#include "labware/kinds/LabwareComponentBase.hpp"
#include "mixtures/Layer.hpp"
#include "structs/Ref.hpp"

#include <SFML/Graphics.hpp>

class MixturePropertyPane : public sf::Drawable
{
	class LayerPropertyPane : public sf::Drawable
	{
	private:
		Ref<const Layer> subject = NullRef;
		sf::Vector2f position = { 0.0f, 0.0f };
		sf::Text title;
		mutable sf::Text propertyName, propertyValue;

	public:
		LayerPropertyPane(const sf::Font& font) noexcept;

		const sf::Vector2f& getPosition() const;

		void setSubject(const Layer& subject);
		void setPosition(const sf::Vector2f& position);

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
	};

	class ContentPropertyPane : public sf::Drawable
	{
	private:
		Ref<const Layer> subject = NullRef;
		sf::Vector2f position = { 0.0f, 0.0f };
		mutable sf::Text propertyName, propertyValue;

	public:
		ContentPropertyPane(const sf::Font& font) noexcept;

		const sf::Vector2f& getPosition() const;

		void setSubject(const Layer& subject);
		void setPosition(const sf::Vector2f& position);

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
	};

private:
	Ref<const LabwareComponentBase> subject = NullRef;
	sf::Vector2f position = { 0.0f, 0.0f };
	sf::RectangleShape background = sf::RectangleShape({ 0.0f, 0.0f });
	sf::Text title;
	mutable sf::Text propertyName, propertyValue;
	mutable LayerPropertyPane layerPane;
	mutable ContentPropertyPane contentPane;

public:
	MixturePropertyPane(const sf::Font& font) noexcept;

	void setSubject(const LabwareComponentBase& subject);
	void setPosition(const sf::Vector2f& position);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
};
