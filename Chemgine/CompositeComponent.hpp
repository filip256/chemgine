#pragma once

#include "BaseComponent.hpp"
#include "MolecularStructure.hpp"

class CompositeComponent : BaseComponent
{
private:
	MolecularStructure structure;
};