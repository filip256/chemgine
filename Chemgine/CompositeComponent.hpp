#pragma once

#include "BaseComponent.hpp"
#include "MolecularStructure.hpp"

class CompositeComponent : public BaseComponent
{
private:
	MolecularStructure structure;

public:
	CompositeComponent(const ComponentType type = ComponentType::COMPOSITE);

	const BaseComponent* getComponent(const size_t idx) const;
};