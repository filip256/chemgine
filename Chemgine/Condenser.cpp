#include "Condenser.hpp"
#include "Flask.hpp"
#include "Adaptor.hpp"

Condenser::Condenser(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, LabwareType::CONDENSER, atmosphere)
{
	getContent<1>().add(Molecule("O"), 10.0_mol);
}

const CondenserData& Condenser::getData() const
{
	return static_cast<const CondenserData&>(data);
}

bool Condenser::tryConnect(BaseLabwareComponent& other)
{
	auto& container = getContent<0>();
	if (other.isFlask())
	{
		container.setAllIncompatibilityTargets(other.as<Flask&>().getContent());
		return true;
	}

	if (other.isAdaptor())
	{
		container.setAllIncompatibilityTargets(other.as<Flask&>().getContent());
		return true;
	}

	if (other.isCondenser())
	{
		container.setAllIncompatibilityTargets(other.as<Flask&>().getContent());
		return true;
	}

	return false;
}

void Condenser::disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other)
{
	this->setOverflowTarget(dump);
}

void Condenser::tick(const Amount<Unit::SECOND> timespan)
{
	const auto& data = getData();
	auto& content = getContent<0>();
	auto& coolant = getContent<1>();

	if (content.isEmpty() == false && coolant.isEmpty() == false)
	{
		// TODO: make hT depend on the temp difference and add conversions methods
		const auto heatTransfer = 5.0_J * data.length.asStd() * data.efficiency.asStd() * timespan.asStd();
		content.add(-heatTransfer);
		coolant.add(heatTransfer);
	}

	ContainerComponent::tick(timespan);
}
