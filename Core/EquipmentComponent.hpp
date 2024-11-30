#pragma once

#include "DrawableComponent.hpp"

class EquipmentComponent : public DrawableComponent
{
protected:
	EquipmentComponent(
		const LabwareId id,
		const LabwareType type
	) noexcept;
};
