#pragma once

#include "ReactionData.hpp"
#include "HashCombine.hpp"

#include <unordered_map>

class RetrosynthReaction
{
private:
	const ReactionData& baseReaction;
	std::unordered_map<Reactable, uint8_t> reactants;
	std::unordered_map<Reactable, uint8_t> products;

public:
	RetrosynthReaction(
		const ReactionData& baseReaction,
		const std::vector<Reactable>& reactants,
		const std::vector<Reactable>& products
	) noexcept;

	RetrosynthReaction(const RetrosynthReaction&) = delete;
	RetrosynthReaction(RetrosynthReaction&&) = default;

	const std::unordered_map<Reactable, uint8_t>& getReactants() const;
	const std::unordered_map<Reactable, uint8_t>& getProducts() const;

	const ReactionData& getBaseData() const;

	std::string getHRTag() const;
	std::string print() const;

	bool operator==(const RetrosynthReaction& other) const;
	bool operator!=(const RetrosynthReaction& other) const;

	friend struct std::hash<RetrosynthReaction>;
};


template<>
struct std::hash<RetrosynthReaction>
{
	size_t operator() (const RetrosynthReaction& reaction) const
	{
		size_t hash = 0;
		for (const auto& [r, _] : reaction.reactants)
			hashCombineWith(hash, r);
		for (const auto& [r, _] : reaction.products)
			hashCombineWith(hash, r);
		return hash;
	}
};
