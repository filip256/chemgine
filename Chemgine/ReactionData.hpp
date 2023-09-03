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
	std::vector<const Reactable*> reactants;
	std::vector<const Reactable*> products;
	std::unordered_map<std::pair<size_t, c_size>, std::pair<size_t, c_size>, PairHash> componentMapping;

	static std::vector<const Reactable*> flatten(
		const std::vector<std::pair<const Reactable*, uint8_t>>& list);

	static bool balance(
		std::vector<std::pair<const Reactable*, uint8_t>>& reactants,
		std::vector<std::pair<const Reactable*, uint8_t>>& products);

	/// <summary>
	/// Maps every component from reactants to every component from products.
	/// Complexity: large
	/// </summary>
	bool mapReactantsToProducts();

public:
	ReactionData(
		const ReactionIdType id,
		const std::string& name,
		const std::vector<std::pair<const Reactable*, uint8_t>>& reactants,
		const std::vector<std::pair<const Reactable*, uint8_t>>& products
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;
	~ReactionData() noexcept;

	bool hasAsReactant(const Molecule& molecule) const;
	
	friend class ReactionDataTable;
};