#pragma once

#include "DataTable.hpp"
#include "ReactionData.hpp"
#include "FunctionalGroupDataTable.hpp"
#include "BackboneDataTable.hpp"
#include "MoleculeDataTable.hpp"

class ReactionDataTable :
	public DataTable<ReactionIdType, std::string, ReactionData>
{
private:
	const FunctionalGroupDataTable& functionalGroups;
	const BackboneDataTable& backbones;
	const MoleculeDataTable& molecules;

	ComponentIdType findComponent(const std::string& smiles) const;
	std::vector<ComponentIdType> findComponents(const std::vector<std::string>& smilesList) const;

public:
	ReactionDataTable(
		const FunctionalGroupDataTable& functionalGroups,
		const BackboneDataTable& backbones,
		const MoleculeDataTable& molecules
	) noexcept;

	ReactionDataTable(const ReactionDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};