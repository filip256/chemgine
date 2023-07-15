#include "ReactionData.hpp"

ReactionData::ReactionData(
	const ReactionIdType id,
	const std::string& name,
	std::vector<ComponentIdType>&& reactants,
	std::vector<ComponentIdType>&& products
) noexcept :
	id(id),
	name(name),
	reactants(std::move(reactants)),
	products(std::move(products))
{}