#pragma once

#include "Repository.hpp"
#include "ReactionData.hpp"
#include "MoleculeRepository.hpp"
#include "ReactionNetwork.hpp"
#include "MoleculeRepository.hpp"

class ReactionRepository
{
private:
	std::unordered_map<ReactionId, std::unique_ptr<ReactionData>> table;

	EstimatorRepository& estimators;
	const MoleculeRepository& molecules;

	uint8_t maxReactantCount = 0;
	ReactionNetwork network;

	ReactionId getFreeId() const;

public:
	ReactionRepository(
		EstimatorRepository& estimators,
		const MoleculeRepository& molecules
	) noexcept;
	ReactionRepository(const ReactionRepository&) = delete;

	bool add(DefinitionObject&& definition);

	using Iterator = std::unordered_map<ReactionId, std::unique_ptr<ReactionData>>::const_iterator;
	Iterator begin() const;
	Iterator end() const;

	size_t size() const;
	void clear();

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
	std::unordered_set<RetrosynthReaction> getRetrosynthReactions(const StructureRef& targetProduct) const;

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
