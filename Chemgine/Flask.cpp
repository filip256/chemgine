#include "Flask.hpp"
#include "DataStore.hpp"
#include "ColorCast.hpp"

Flask::Flask(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	DrawableComponent(id),
	content(atmosphere, getData().volume, atmosphere)
{}

Flask::Flask(const Flask& other) noexcept :
	DrawableComponent(other.id),
	content(other.content.makeCopy())
{}

const FlaskData& Flask::getData() const
{
	return static_cast<const FlaskData&>(data);
}

void Flask::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	content.add(molecule, amount);
}

void Flask::tick()
{
	content.tick();
}

void Flask::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	float lastSection = 0.0f;
	for (auto l = content.getLayersUpBegin(); l != content.getLayersUpEnd(); ++l)
	{
		const auto layerSection = (l->second.getVolume() / content.getMaxVolume()).asStd();
		fill.setDrawSection(lastSection, layerSection, colorCast(l->second.getColor()));
		lastSection += layerSection;

		target.draw(fill);
	}

	DrawableComponent::draw(target, states);
}

Flask* Flask::clone() const
{
	return new Flask(*this);
}
