#pragma once

#include <vector>

#include "BaseComponentData.hpp"

typedef uint16_t ReactionIdType;

class ReactionData
{
private:
	const ReactionIdType id;
	const std::string name;
	std::vector<ComponentIdType> reactants;
	std::vector<ComponentIdType> products;

public:
	ReactionData(
		const ReactionIdType id,
		const std::string& name,
		std::vector<ComponentIdType>&& reactants,
		std::vector<ComponentIdType>&& products
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;
	~ReactionData() = default;
};