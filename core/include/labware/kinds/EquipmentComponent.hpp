#pragma once

#include "labware/kinds/DrawableComponent.hpp"

class EquipmentComponent : public DrawableComponent
{
protected:
	EquipmentComponent(
		const LabwareId id,
		const LabwareType type
	) noexcept;
};
