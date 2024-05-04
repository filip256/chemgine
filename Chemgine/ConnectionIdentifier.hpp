#pragma once

#include "BaseLabwareComponent.hpp"

class LabwareSystem;

class PortIdentifier
{
private:
	l_size componentIdx;
	uint8_t portIdx;
	LabwareSystem& system;

	PortIdentifier(
		LabwareSystem& system,
		const l_size componentIdx,
		const uint8_t portIdx
	) noexcept;

public:
	bool isValid() const;
	l_size getComponentIndex() const;
	uint8_t getPortIndex() const;
	const BaseLabwareComponent& getComponent() const;
	BaseLabwareComponent& getComponent();
	const LabwareSystem& getSystem() const;
	LabwareSystem& getSystem();

	const DrawableLabwareConnection* operator->() const;

	friend class LabwareSystem;
};
