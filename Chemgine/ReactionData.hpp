#pragma once

#include <vector>

#include "BaseComponentData.hpp"
#include "Reactable.hpp"

typedef uint16_t ReactionIdType;

class ReactionData
{
private:
	const ReactionIdType id;
	const std::string name;
	std::vector<std::pair<const Reactable*, uint8_t>> reactants;
	std::vector<std::pair<const Reactable*, uint8_t>> products;


public:
	ReactionData(
		const ReactionIdType id,
		const std::string& name,
		std::vector<std::pair<const Reactable*, uint8_t>>&& reactants,
		std::vector<std::pair<const Reactable*, uint8_t>>&& products
	) noexcept;

	ReactionData(const ReactionData&) = delete;
	ReactionData(ReactionData&&) = default;
	~ReactionData() noexcept;
	
	bool balance();
};