#pragma once

#include "BaseComponentData.hpp"
#include "MolecularStructure.hpp"

class FunctionalGroupData : public BaseComponentData
{
private:
	MolecularStructure structure;

public:
	FunctionalGroupData(
		const ComponentIdType id,
		const std::string& name,
		MolecularStructure&& structure) noexcept;

	FunctionalGroupData(const FunctionalGroupData&) = delete;
	FunctionalGroupData(FunctionalGroupData&&) = default;
	~FunctionalGroupData() = default;
};