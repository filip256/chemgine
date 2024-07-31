#pragma once

#include <string>
#include <vector>

class ReactionSpecifier
{
public:
	const std::vector<std::string> reactants;
	const std::vector<std::string> products;

	ReactionSpecifier(
		std::vector<std::string>&& reactants,
		std::vector<std::string>&& products
	) noexcept;
	ReactionSpecifier(const ReactionSpecifier&) = delete;
	ReactionSpecifier(ReactionSpecifier&&) = default;
};
