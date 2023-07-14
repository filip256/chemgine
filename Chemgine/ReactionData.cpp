#include "ReactionData.hpp"

ReactionData::ReactionData(
	const ReactionIdType id,
	std::vector<ComponentIdType>&& reactants,
	std::vector<ComponentIdType>&& products
) noexcept :
	id(id),
	reactants(std::move(reactants)),
	products(std::move(products))
{}