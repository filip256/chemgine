#pragma once

#include <vector>

#include "BaseComponentData.hpp"
#include "Reactable.hpp"
#include "PairHash.hpp"
#include "Molecule.hpp"

typedef uint16_t ReactionIdType;

class ReactionData
{
private:
	const ReactionIdType id;
	const std::string name;
	std::vector<Reactable> reactants;
	std::vector<Reactable> products;
	std::unordered_map<std::pair<size_t, c_size>, std::pair<size_t, c_size>, PairHash> componentMapping;

	static std::vector<Reactable> flatten(
		const std::vector<std::pair<Reactable, uint8_t>>& list);

	static bool balance(
		std::vector<std::pair<Reactable, uint8_t>>& reactants,
		std::vector<std::pair<Reactable, uint8_t>>& products);


	void enumerateReactantPairs(
		const std::vector<Molecule>& molecules,
		const std::unordered_set<std::pair<size_t, size_t>, PairHash>& allowedPairs,
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
	ReactionData(
		const ReactionIdType id,
		const std::string& name,
		const std::vector<std::pair<Reactable, uint8_t>>& reactants,
		const std::vector<std::pair<Reactable, uint8_t>>& products
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;

	bool hasAsReactant(const Molecule& molecule) const;

	/// <summary>
	/// If the given vector of molecules matches the reactant list, it returns the resulting concrete
	/// products, otherwise it returns an empty vector.
	/// </summary>
	std::vector<Molecule> generateConcreteProducts(const std::vector<Molecule>& molecules) const;

	const std::vector<Reactable>& getReactants() const;
	const std::vector<Reactable>& getProducts() const;


	friend class ReactionDataTable;
};