#pragma once

#include <vector>

#include "BaseComponentData.hpp"
#include "Reactable.hpp"
#include "PairHash.hpp"
#include "Reactant.hpp"
#include "Catalyst.hpp"
#include "Amount.hpp"
#include "Ref.hpp"

typedef uint16_t ReactionId;

class ReactionData
{
private:
	Ref<const ReactionData> baseReaction = nullRef;
	std::vector<Reactable> reactants;
	std::vector<Reactable> products;
	std::vector<Catalyst> catalysts;
	std::unordered_map<std::pair<size_t, c_size>, std::pair<size_t, c_size>> componentMapping;
	
	static std::vector<Reactable> flatten(
		const std::vector<std::pair<Reactable, uint8_t>>& list);

	static bool balance(
		std::vector<std::pair<Reactable, uint8_t>>& reactants,
		std::vector<std::pair<Reactable, uint8_t>>& products);

	void enumerateReactantPairs(
		const std::vector<Molecule>& molecules,
		const std::unordered_set<std::pair<size_t, size_t>>& allowedPairs,
		std::vector<std::pair<size_t, size_t>>& currentMatch,
		std::vector<std::vector<std::pair<size_t, size_t>>>& result) const;

	/// <summary>
	/// Maps every component from reactants to every component from products.
	/// Complexity: large
	/// </summary>
	bool mapReactantsToProducts();

	/// <summary>
	/// Returns a vector where each element [i] is a vector containing all the successful matchings 
	/// between the i^th unique reactant and a molecule from the vector given as parameter.
	/// </summary>
	std::vector<std::vector<std::pair<size_t, std::unordered_map<c_size, c_size>>>> mapReactantsToMolecules(
		const std::vector<Molecule>& molecules) const;

public:
	const ReactionId id;
	const Amount<Unit::MOLE_PER_SECOND> baseSpeed;
	const Amount<Unit::CELSIUS> baseTemperature;
	const Amount<Unit::JOULE_PER_MOLE> reactionEnergy;
	const Amount<Unit::JOULE_PER_MOLE> activationEnergy;
	const std::string name;

	ReactionData(
		const ReactionId id,
		const std::string& name,
		const std::vector<std::pair<Reactable, uint8_t>>& reactants,
		const std::vector<std::pair<Reactable, uint8_t>>& products,
		const Amount<Unit::MOLE_PER_SECOND> baseSpeed,
		const Amount<Unit::CELSIUS> baseTemperature,
		const Amount<Unit::JOULE_PER_MOLE> reactionEnergy,
		const Amount<Unit::JOULE_PER_MOLE> activationEnergy,
		std::vector<Catalyst>&& catalysts
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;

	bool hasAsReactant(const Reactant& reactant) const;
	
	/// <summary>
	/// Tries to map the i-th molecule in the given vector with i-th reactant of the reaction.
	/// If successful, a non-empty vector of atom maps is returned.
	/// </summary>
	std::vector<std::unordered_map<c_size, c_size>> generateConcreteMatches(const std::vector<Reactant>& molecules) const;
	/// <summary>
	/// Generetates the concrete products of the reaction for the given molecules, using the matches
	/// resulted from generateConcreteMatches(const std::vector<Reactant>&).
	/// </summary>
	std::vector<Molecule> generateConcreteProducts(
		const std::vector<Reactant>& molecules,
		const std::vector<std::unordered_map<c_size, c_size>>& matches) const;

	const std::vector<Reactable>& getReactants() const;
	const std::vector<Reactable>& getProducts() const;
	const std::vector<Catalyst>& getCatalysts() const;

	bool isSpecializationOf(const ReactionData& other) const;
	bool isGeneralizationOf(const ReactionData& other) const;
	bool isEquivalentTo(const ReactionData& other) const;

	void setBaseReaction(const ReactionData& reaction);

	std::string getHRTag() const;

	friend class ReactionRepository;
};
