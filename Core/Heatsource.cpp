#include "Heatsource.hpp"

Heatsource::Heatsource(
	const LabwareId id,
	AtmosphereMixture& atmosphere
) noexcept :
	EquipmentComponent(id, LabwareType::HEATSOURCE),
	target(atmosphere)
{}

const HeatsourceData& Heatsource::getData() const
{
	return static_cast<const HeatsourceData&>(data);
}

void Heatsource::setTarget(const Ref<ContainerBase> target)
{
	this->target = target;
}

void Heatsource::setTarget(BaseContainerComponent& target)
{
	this->target = target.getContent();
}

bool Heatsource::tryConnect(LabwareComponentBase& other)
{
	if (other.isContainer())
	{
		this->setTarget(other.as<BaseContainerComponent&>());
		return true;
	}

	return false;
}

void Heatsource::disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other)
{
	this->setTarget(dump);
}

void Heatsource::tick(const Amount<Unit::SECOND> timespan)
{
	if (target.isSet() == false)
		return;

	target->add(getData().maxPowerOutput.to<Unit::JOULE>(timespan));
}
