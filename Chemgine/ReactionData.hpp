#pragma once

#include <vector>

#include "BaseComponentData.hpp"
#include "ImmutableSet.hpp"
#include "Reactable.hpp"
#include "PairHash.hpp"
#include "Reactant.hpp"
#include "Catalyst.hpp"
#include "Amount.hpp"
#include "Ref.hpp"

typedef uint16_t ReactionId;

class RetrosynthReaction;

class ReactionData
{
private:
	Ref<const ReactionData> baseReaction = nullRef;
	std::vector<Reactable> reactants;
	std::vector<Reactable> products;
	ImmutableSet<Catalyst> catalysts;
	std::unordered_map<std::pair<size_t, c_size>, std::pair<size_t, c_size>> componentMapping;

	static bool balance(
		std::vector<std::pair<Reactable, uint8_t>>& reactants,
		std::vector<std::pair<Reactable, uint8_t>>& products);

	/// <summary>
	/// Maps every component from reactants to every component from products.
	/// Complexity: large
	/// </summary>
	bool mapReactantsToProducts();

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
		ImmutableSet<Catalyst>&& catalysts
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;

	bool hasAsReactant(const Reactant& reactant) const;
	
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
	std::pair<size_t, std::unordered_map<c_size, c_size>> generateRetrosynthProductMatches(const Reactable& targetProduct) const;
	
	/// <summary>
	/// Generetates the concrete products of the reaction for the given molecules, using the matches
	/// resulted from generateConcreteReactantMatches(...).
	/// </summary>
	std::vector<Molecule> generateConcreteProducts(
		const std::vector<Reactant>& molecules,
		const std::vector<std::unordered_map<c_size, c_size>>& matches) const;

	/// <summary>
	/// Generetates the concrete reactants of the reaction leading to the given target, using the matches
	/// resulted from generateConcreteProductMatches(...).
	/// Radical atoms which unmatched by the target remain unsubstituted in the resulted reactants.
	/// </summary>
	RetrosynthReaction generateRetrosynthReaction(
		const Reactable& targetProduct,
		const std::pair<size_t, std::unordered_map<c_size, c_size>>& match) const;

	const std::vector<Reactable>& getReactants() const;
	const std::vector<Reactable>& getProducts() const;
	const ImmutableSet<Catalyst>& getCatalysts() const;

	bool isCutReaction() const;
	bool isSpecializationOf(const ReactionData& other) const;
	bool isGeneralizationOf(const ReactionData& other) const;
	bool isEquivalentTo(const ReactionData& other) const;

	void setBaseReaction(const ReactionData& reaction);

	std::string getHRTag() const;

	static constexpr size_t npos = static_cast<size_t>(-1);

	friend class ReactionRepository;
};
