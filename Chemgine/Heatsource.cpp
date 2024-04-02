#include "Heatsource.hpp"

Heatsource::Heatsource(const LabwareId id) noexcept :
	EquipmentComponent(id)
{}

const HeatsourceData& Heatsource::getData() const
{
	return static_cast<const HeatsourceData&>(data);
}

void Heatsource::setTarget(const Ref<Mixture> target)
{
	this->target = target;
}

void Heatsource::setTarget(BaseContainerComponent& target)
{
	this->target = target.getContent();
}

void Heatsource::tick(const Amount<Unit::SECOND> timespan)
{
	if (target.isSet() == false)
		return;

	target->add(getData().maxPowerOutput.to<Unit::JOULE>(timespan));
}
