#pragma once

#include "BaseComponentData.hpp"
#include "MolecularStructure.hpp"

class FunctionalGroupData : public BaseComponentData
{
private:
	MolecularStructure structure;

	FunctionalGroupData(
		const ComponentIdType id,
		const std::string& name,
		MolecularStructure&& structure) noexcept;

public:

	FunctionalGroupData(const FunctionalGroupData&) = delete;
	FunctionalGroupData(FunctionalGroupData&&) = default;
	~FunctionalGroupData() = default;

	static FunctionalGroupData create(
		const ComponentIdType id,
		const std::string& name,
		const std::string& smiles);
};