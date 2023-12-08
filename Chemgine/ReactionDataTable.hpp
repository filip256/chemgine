#pragma once

#include "DataTable.hpp"
#include "ReactionData.hpp"
#include "FunctionalGroupDataTable.hpp"
#include "BackboneDataTable.hpp"
#include "MoleculeDataTable.hpp"
#include "ConcreteReaction.hpp"

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

	/// <summary>
	/// Finds all the occuring reactions for the given molecules. The order of molecules must match the order of the
	/// reactants in the matching reaction.
	/// </summary>
	std::unordered_set<ConcreteReaction, ConcreteReactionHash> findOccuringReactions(const std::vector<Molecule>& molecules) const;
};