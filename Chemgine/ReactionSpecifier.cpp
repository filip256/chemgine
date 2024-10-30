#include "ReactionSpecifier.hpp"

using namespace Def;

ReactionSpecifier::ReactionSpecifier(
	std::vector<std::string>&& reactants,
	std::vector<std::string>&& products
) noexcept :
	reactants(std::move(reactants)),
	products(std::move(products))
{}
