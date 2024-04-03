#pragma once

#include "Repository.hpp"
#include "ReactionData.hpp"
#include "GenericMoleculeRepository.hpp"
#include "MoleculeRepository.hpp"
#include "ReactionNetwork.hpp"

class ReactionRepository :
	public Repository<ReactionId, std::string, ReactionData>
{
private:
	ReactionNetwork network;

public:
	ReactionRepository() = default;

	ReactionRepository(const ReactionRepository&) = delete;

	bool loadFromFile(const std::string& path);

	const ReactionNetwork& getNetwork() const;

	/// <summary>
	/// Finds all the occuring reactions for the given molecules. The order of molecules must match the order of the
	/// reactants in the matching reaction.
	/// </summary>
	std::unordered_set<ConcreteReaction> findOccuringReactions(const std::vector<Reactant>& reactants) const;
};
