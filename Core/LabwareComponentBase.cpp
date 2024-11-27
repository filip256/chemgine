#include "LabwareComponentBase.hpp"
#include "DataStore.hpp"
#include "Log.hpp"

LabwareComponentBase::LabwareComponentBase(
	const LabwareId id,
	const LabwareType type
) noexcept :
	id(id),
	data(Accessor<>::getDataStore().labware.at(id))
{
	if (type != data.type)
		Log(this).fatal("Labware component given by id {0} does not match the requested component type.", id);
}

bool LabwareComponentBase::isFlask() const
{
	return data.type == LabwareType::FLASK;
}

bool LabwareComponentBase::isAdaptor() const
{
	return data.type == LabwareType::ADAPTOR;
}

bool LabwareComponentBase::isCondenser() const
{
	return data.type == LabwareType::CONDENSER;
}

bool LabwareComponentBase::isHeatsource() const
{
	return data.type == LabwareType::HEATSOURCE;
}

bool LabwareComponentBase::isContainer() const
{
	return false;
}

bool LabwareComponentBase::tryConnect(LabwareComponentBase& other)
{
	return false;
}

void LabwareComponentBase::disconnect(Ref<ContainerBase> dump, const LabwareComponentBase& other)
{}

void LabwareComponentBase::tick(const Amount<Unit::SECOND> timespan)
{}
