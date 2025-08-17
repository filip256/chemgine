#pragma once

#include "atomics/data/BaseComponentData.hpp"
#include "estimators/kinds/UnitizedEstimator.hpp"
#include "structs/ImmutableSet.hpp"
#include "reactions/StructureRef.hpp"
#include "utils/Hash.hpp"
#include "reactions/Reactant.hpp"
#include "reactions/Catalyst.hpp"
#include "data/values/Amount.hpp"
#include "structs/Ref.hpp"

#include <vector>

typedef uint16_t ReactionId;

class RetrosynthReaction;

class ReactionData
{
private:
	static bool balance(
		std::vector<std::pair<StructureRef, uint8_t>>& reactants,
		std::vector<std::pair<StructureRef, uint8_t>>& products);

	/// <summary>
	/// Maps every component from reactants to every component from products.
	/// Complexity: large
	/// </summary>
	bool mapReactantsToProducts();

public:
	const ReactionId id;
	const bool isCut;
	const Amount<Unit::JOULE_PER_MOLE> reactionEnergy;
	const Amount<Unit::JOULE_PER_MOLE> activationEnergy;
	const std::string name;
	const EstimatorRef<Unit::MOLE_PER_SECOND, Unit::CELSIUS> tempSpeedEstimator;
	const EstimatorRef<Unit::NONE, Unit::MOLE_RATIO> concSpeedEstimator;

private:
	Ref<const ReactionData> baseReaction = NullRef;
	std::vector<StructureRef> reactants;
	std::vector<StructureRef> products;
	ImmutableSet<Catalyst> catalysts;
	std::unordered_map<std::pair<size_t, c_size>, std::pair<size_t, c_size>> componentMapping;

public:
	ReactionData(
		const ReactionId id,
		const std::string& name,
		const std::vector<std::pair<StructureRef, uint8_t>>& reactants,
		const std::vector<std::pair<StructureRef, uint8_t>>& products,
		const Amount<Unit::JOULE_PER_MOLE> reactionEnergy,
		const Amount<Unit::JOULE_PER_MOLE> activationEnergy,
		EstimatorRef<Unit::MOLE_PER_SECOND, Unit::CELSIUS>&& tempSpeedEstimator,
		EstimatorRef<Unit::NONE, Unit::MOLE_RATIO>&& concSpeedEstimator,
		ImmutableSet<Catalyst>&& catalysts
	) noexcept;

	ReactionData(
		const ReactionId id,
		const std::string& name,
		const std::vector<std::pair<StructureRef, uint8_t>>& reactants,
		const std::vector<std::pair<StructureRef, uint8_t>>& products,
		EstimatorRef<Unit::MOLE_PER_SECOND, Unit::CELSIUS>&& tempSpeedEstimator,
		EstimatorRef<Unit::NONE, Unit::MOLE_RATIO>&& concSpeedEstimator,
		ImmutableSet<Catalyst>&& catalysts
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;

	bool hasAsReactant(const MolecularStructure& structure) const;
	bool hasAsReactant(const Molecule& molecule) const;
	
	/// <summary>
	/// Tries to map the i-th molecule in the given vector with i-th reactant of the reaction.
	/// If successful, a non-empty vector of atom maps is returned.
	/// </summary>
	std::vector<std::unordered_map<c_size, c_size>> generateConcreteReactantMatches(const std::vector<Reactant>& molecules) const;
	
	/// <summary>
	/// Tries to map one of the products of the reaction to the given target molecule.
	/// If successful, the index of the product and an atom map are returned,
	/// otherwise npos is retuned.
	/// </summary>
	std::pair<size_t, std::unordered_map<c_size, c_size>> generateRetrosynthProductMatches(const StructureRef& targetProduct) const;
	
	/// <summary>
	/// Generates the concrete products of the reaction for the given molecules, using the matches
	/// resulted from generateConcreteReactantMatches(...).
	/// </summary>
	std::vector<Molecule> generateConcreteProducts(
		const std::vector<Reactant>& molecules,
		const std::vector<std::unordered_map<c_size, c_size>>& matches) const;

	/// <summary>
	/// Generates the concrete reactants of the reaction leading to the given target, using the matches
	/// resulted from generateConcreteProductMatches(...).
	/// Radical atoms which unmatched by the target remain unsubstituted in the resulted reactants.
	/// </summary>
	RetrosynthReaction generateRetrosynthReaction(
		const StructureRef& targetProduct,
		const std::pair<size_t, std::unordered_map<c_size, c_size>>& match) const;

	const std::vector<StructureRef>& getReactants() const;
	const std::vector<StructureRef>& getProducts() const;
	const ImmutableSet<Catalyst>& getCatalysts() const;

	Amount<Unit::MOLE_PER_SECOND> getSpeedAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::MOLE_RATIO> concentration
	) const;

	bool isCutReaction() const;
	bool isSpecializationOf(const ReactionData& other) const;
	bool isGeneralizationOf(const ReactionData& other) const;
	bool isEquivalentTo(const ReactionData& other) const;

	void setBaseReaction(const ReactionData& reaction);

	std::string getHRTag() const;

	void dumpDefinition(
		std::ostream& out,
		const bool prettify,
		std::unordered_set<EstimatorId>& alreadyPrinted
	) const;
	void print(std::ostream& out = std::cout) const;

	static constexpr size_t npos = static_cast<size_t>(-1);

	friend class ReactionRepository;
};
