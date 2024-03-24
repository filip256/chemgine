#include "Heatsource.hpp"

Heatsource::Heatsource(const LabwareId id) noexcept :
	EquipmentComponent(id)
{}

const HeatsourceData& Heatsource::getData() const
{
	return static_cast<const HeatsourceData&>(data);
}
