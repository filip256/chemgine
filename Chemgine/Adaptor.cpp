#include "Adaptor.hpp"
#include "ColorCast.hpp"

Adaptor::Adaptor(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	DrawableComponent(id),
	content(atmosphere.createSubatmosphere(getData().volume))
{}

Adaptor::Adaptor(const Adaptor& other) noexcept :
	DrawableComponent(other.id),
	content(other.content.makeCopy())
{}


const AdaptorData& Adaptor::getData() const
{
	return static_cast<const AdaptorData&>(data);
}

void Adaptor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	fill.setColor(colorCast(content.getLayerColor()));
	target.draw(fill);

	DrawableComponent::draw(target, states);
}

Adaptor* Adaptor::clone() const
{
	return new Adaptor(*this);
}
