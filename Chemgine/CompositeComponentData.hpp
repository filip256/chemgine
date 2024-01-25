#pragma once

#include "BaseComponentData.hpp"
#include "MolecularStructure.hpp"

class CompositeComponentData : public BaseComponentData
{
protected:
	MolecularStructure structure;

	CompositeComponentData(
		const ComponentIdType id,
		MolecularStructure&& structure) noexcept;

	CompositeComponentData(const CompositeComponentData&) = delete;
	CompositeComponentData(CompositeComponentData&&) = default;

	static CompositeComponentData create(
		const ComponentIdType id,
		const std::string& smiles);

public:
	~CompositeComponentData() = default;

	const MolecularStructure& getStructure() const;

	uint8_t getFittingValence(const uint8_t bonds) const override final;
	std::string getSMILES() const override final;
	std::string getBinaryId() const override final;
};