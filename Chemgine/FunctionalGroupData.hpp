#pragma once

#include "BaseComponentData.hpp"
#include "MolecularStructure.hpp"

class FunctionalGroupData : public BaseComponentData
{
public:
	const MolecularStructure structure;

	FunctionalGroupData(
		const ComponentIdType id,
		const std::string& symbol,
		const std::string& name,
		MolecularStructure&& structure);
};