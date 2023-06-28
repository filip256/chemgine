#pragma once

#include "BaseComponent.hpp"
#include "MolecularStructure.hpp"

class CompositeComponent : public BaseComponent
{
private:

protected:
	CompositeComponent(
		const std::string& smiles,
		const ComponentType type
	);

	MolecularStructure structure;

public:
	const BaseComponent* getComponent(const size_t idx) const;
};