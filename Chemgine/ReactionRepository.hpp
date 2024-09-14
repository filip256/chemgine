#pragma once

#include "Repository.hpp"
#include "ReactionData.hpp"
#include "GenericMoleculeRepository.hpp"
#include "MoleculeRepository.hpp"
#include "ReactionNetwork.hpp"
#include "MoleculeRepository.hpp"

class ReactionRepository :
	public Repository<ReactionId, std::string, ReactionData>
{
private:
	EstimatorRepository& estimators;
	const MoleculeRepository& molecules;

	uint8_t maxReactantCount = 0;
	ReactionNetwork network;

public:
	ReactionRepository(
		EstimatorRepository& estimators,
		const MoleculeRepository& molecules
	) noexcept;
	ReactionRepository(const ReactionRepository&) = delete;

	bool add(DefinitionObject&& definition);

	void buildNetwork();

	uint8_t getMaxReactantCount() const;
	const ReactionNetwork& getNetwork() const;

	/// <summary>
	/// Finds all the occuring reactions for the given molecules. The order of molecules must match the order of the
	/// reactants in the matching reaction.
	/// </summary>
	std::unordered_set<ConcreteReaction> findOccuringReactions(const std::vector<Reactant>& reactants) const;

	/// <summary>
	/// Finds all the reactions which can produce the given target and specializes them accordingly.
	/// </summary>
	std::unordered_set<RetrosynthReaction> getRetrosynthReactions(const Reactable& targetProduct) const;

	/// <summary>
	/// Generates all the molecules which can be reached using the existing molecules and reactions.
	/// Returns the number of new molecules found.
	/// </summary>
	size_t generateCurrentSpan() const;

	/// <summary>
	/// Generates all the molecules which can be reached using the existing molecules and reactions.
	/// Newely created molecules are also taken into account in further iterations.
	/// Returns the number of new molecules found.
	/// </summary>
	size_t generateTotalSpan(const size_t maxIterations = std::numeric_limits<size_t>::max()) const;
};
