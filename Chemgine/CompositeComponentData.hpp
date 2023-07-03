#pragma once

#include "BaseComponentData.hpp"
#include "MolecularStructure.hpp"

class CompositeComponentData : public BaseComponentData
{
protected:
	MolecularStructure structure;

	CompositeComponentData(
		const ComponentIdType id,
		const std::string& name,
		MolecularStructure&& structure) noexcept;

	CompositeComponentData(const CompositeComponentData&) = delete;
	CompositeComponentData(CompositeComponentData&&) = default;

	static CompositeComponentData create(
		const ComponentIdType id,
		const std::string& name,
		const std::string& smiles);

public:
	~CompositeComponentData() = default;

	const MolecularStructure& getStructure() const;
};