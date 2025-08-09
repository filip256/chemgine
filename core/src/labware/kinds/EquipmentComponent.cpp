#include "labware/kinds/EquipmentComponent.hpp"

EquipmentComponent::EquipmentComponent(
	const LabwareId id,
	const LabwareType type
) noexcept :
	DrawableComponent(id, type)
{}
