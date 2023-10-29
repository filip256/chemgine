#pragma once

#include "DataTable.hpp"
#include "ReactionData.hpp"
#include "FunctionalGroupDataTable.hpp"
#include "BackboneDataTable.hpp"
#include "MoleculeDataTable.hpp"
#include "Reaction.hpp"

class ReactionDataTable :
	public DataTable<ReactionIdType, std::string, ReactionData>
{
private:
	const FunctionalGroupDataTable& functionalGroups;
	const BackboneDataTable& backbones;
	const MoleculeDataTable& molecules;

public:
	ReactionDataTable(
		const FunctionalGroupDataTable& functionalGroups,
		const BackboneDataTable& backbones,
		const MoleculeDataTable& molecules
	) noexcept;

	ReactionDataTable(const ReactionDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	void findOccuringReactions(const std::vector<Molecule>& molecules) const;
};