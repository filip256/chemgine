#include "Adaptor.hpp"
#include "DataStore.hpp"
#include "ColorCast.hpp"

Adaptor::Adaptor(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, atmosphere)
{}

const AdaptorData& Adaptor::getData() const
{
	return static_cast<const AdaptorData&>(data);
}

void Adaptor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	fill.setColor(colorCast(container.getLayerColor()));
	target.draw(fill);

	ContainerComponent::draw(target, states);
}
