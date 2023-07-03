#pragma once

#include "BaseComponent.hpp"

class CompositeComponent : public BaseComponent
{
private:

protected:
	CompositeComponent(const ComponentIdType id, const ComponentType type);

public:
	virtual const BaseComponent* getComponent(const size_t idx) const = 0;
};