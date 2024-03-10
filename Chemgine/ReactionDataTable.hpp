#pragma once

#include "DataTable.hpp"
#include "ReactionData.hpp"
#include "GenericMoleculeDataTable.hpp"
#include "MoleculeDataTable.hpp"
#include "ReactionNetwork.hpp"

class ReactionDataTable :
	public DataTable<ReactionId, std::string, ReactionData>
{
private:
	ReactionNetwork network;

public:
	ReactionDataTable() = default;

	ReactionDataTable(const ReactionDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	const ReactionNetwork& getNetwork() const;

	/// <summary>
	/// Finds all the occuring reactions for the given molecules. The order of molecules must match the order of the
	/// reactants in the matching reaction.
	/// </summary>
	std::unordered_set<ConcreteReaction> findOccuringReactions(const std::vector<Reactant>& reactants) const;
};