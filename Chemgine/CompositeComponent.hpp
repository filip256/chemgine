#pragma once

#include "BaseComponent.hpp"

class CompositeComponent : public BaseComponent
{
private:

protected:
	CompositeComponent(const ComponentIdType id, const ComponentType type);

public:
	virtual const BaseComponent* getComponent(const c_size idx) const = 0;

	bool isRadicalType() const override final;
};