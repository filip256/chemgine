#pragma once

#include "BaseLabwareComponent.hpp"
#include "PVector.hpp"

class LabwareSystem
{
private:
	PVector<BaseLabwareComponent, uint8_t> components;

public:
	LabwareSystem() = default;
	LabwareSystem(const LabwareSystem&) = delete;
	LabwareSystem(LabwareSystem&&) = default;
};