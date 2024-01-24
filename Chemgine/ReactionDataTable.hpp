#pragma once

#include "DataTable.hpp"
#include "ReactionData.hpp"
#include "GenericMoleculeDataTable.hpp"
#include "MoleculeDataTable.hpp"
#include "ConcreteReaction.hpp"

class ReactionDataTable :
	public DataTable<ReactionIdType, std::string, ReactionData>
{
public:
	ReactionDataTable() = default;

	ReactionDataTable(const ReactionDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	/// <summary>
	/// Finds all the occuring reactions for the given molecules. The order of molecules must match the order of the
	/// reactants in the matching reaction.
	/// </summary>
	std::unordered_set<ConcreteReaction, ConcreteReactionHash> findOccuringReactions(const std::vector<Reactant>& reactants) const;
};