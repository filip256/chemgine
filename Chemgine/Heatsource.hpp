#pragma once

#include "EquipmentComponent.hpp"
#include "HeatsourceData.hpp"

class Heatsource : public EquipmentComponent
{
public:
	Heatsource(const LabwareId id) noexcept;

	const HeatsourceData& getData() const override final;
};
