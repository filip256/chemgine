#include "Adaptor.hpp"
#include "DataStore.hpp"
#include "ColorCast.hpp"
#include "Flask.hpp"

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

bool Adaptor::tryConnect(BaseLabwareComponent& other)
{
	if (other.isFlask())
	{
		this->container.setAllIncompatibilityTargets(other.as<Flask&>().getContent());
		return true;
	}

	if (other.isAdaptor())
	{
		this->container.setOverflowTarget(other.as<Adaptor&>().getContent());
		return true;
	}

	return false;
}

void Adaptor::disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other)
{
	if (other.isFlask())
		this->container.setAllIncompatibilityTargets(dump);

	if (other.isAdaptor())
		this->container.setOverflowTarget(dump);
}

void Adaptor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	fill.setColor(colorCast(container.getLayerColor()));
	target.draw(fill);

	ContainerComponent::draw(target, states);
}
