#include "Flask.hpp"
#include "DataStore.hpp"

Flask::Flask(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, LabwareType::FLASK, atmosphere)
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
