#pragma once

#include "SFML/Graphics.hpp"
#include "LabwareComponentBase.hpp"
#include "Layer.hpp"
#include "Ref.hpp"

class MixturePropertyPane : public sf::Drawable
{
	class LayerPropertyPane : public sf::Drawable
	{
	private:
		Ref<const Layer> subject = nullRef;
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
		Ref<const Layer> subject = nullRef;
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
	Ref<const LabwareComponentBase> subject = nullRef;
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
