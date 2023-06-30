#pragma once

#include "BaseComponent.hpp"

class AtomicComponent : public BaseComponent
{
protected:
	AtomicComponent(const ComponentType type);

public:
	virtual bool isRadicalType() const = 0;
};