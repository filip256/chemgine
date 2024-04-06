#include "Flask.hpp"
#include "DataStore.hpp"
#include "ColorCast.hpp"

Flask::Flask(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, atmosphere)
{}

const FlaskData& Flask::getData() const
{
	return static_cast<const FlaskData&>(data);
}

bool Flask::tryConnect(BaseLabwareComponent& other)
{
	if (other.isContainer())
	{
		this->setOverflowTarget(other.as<BaseContainerComponent&>());
		return true;
	}

	return false;
}

void Flask::disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other)
{
	this->setOverflowTarget(dump);
}

void Flask::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	float lastSection = 0.0f;
	for (auto l = container.getLayersUpBegin(); l != container.getLayersUpEnd(); ++l)
	{
		const auto layerSection = (l->second.getVolume() / container.getMaxVolume()).asStd();
		fill.setDrawSection(lastSection, lastSection + layerSection, colorCast(l->second.getColor()));
		lastSection += layerSection;

		target.draw(fill);
	}

	ContainerComponent::draw(target, states);
}
