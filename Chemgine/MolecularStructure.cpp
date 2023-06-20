#include "MolecularStructure.hpp"

const BaseComponent* MolecularStructure::getComponent(const size_t idx) const
{
	return components[idx];
}