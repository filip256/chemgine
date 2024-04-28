#include "Heatsource.hpp"

Heatsource::Heatsource(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	EquipmentComponent(id, LabwareType::HEATSOURCE),
	target(atmosphere)
{}

const HeatsourceData& Heatsource::getData() const
{
	return static_cast<const HeatsourceData&>(data);
}

void Heatsource::setTarget(const Ref<BaseContainer> target)
{
	this->target = target;
}

void Heatsource::setTarget(BaseContainerComponent& target)
{
	this->target = target.getContent();
}

bool Heatsource::tryConnect(BaseLabwareComponent& other)
{
	if (other.isContainer())
	{
		this->setTarget(other.as<BaseContainerComponent&>());
		return true;
	}

	return false;
}

void Heatsource::disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other)
{
	this->setTarget(dump);
}

void Heatsource::tick(const Amount<Unit::SECOND> timespan)
{
	if (target.isSet() == false)
		return;

	target->add(getData().maxPowerOutput.to<Unit::JOULE>(timespan));
}
