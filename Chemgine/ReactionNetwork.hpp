#pragma once

#include "ReactionData.hpp"
#include "DirectedGraph.hpp"
#include "TextBlock.hpp"
#include "ConcreteReaction.hpp"
#include "RetrosynthReaction.hpp"

class ReactionNetwork
{
	class ReactionNode
	{
	public:
		ReactionData& data;

		ReactionNode(ReactionData& data) noexcept;
		ReactionNode(const ReactionNode&) = default;
	};

private:
	DirectedGraph<ReactionNode> graph;
	std::vector<size_t> topLayer;

	bool insert(const size_t current, ReactionData& reaction, size_t& firstInsert);

	bool getOccuringReactions(
		const std::vector<Reactant>& reactants,
		const size_t current,
		std::unordered_set<ConcreteReaction>& result) const;

	bool getRetrosynthReactions(
		const StructureRef& targetProduct,
		const size_t current,
		std::unordered_set<RetrosynthReaction>& result) const;

	void print(const size_t current, TextBlock& block, size_t& y, std::vector<uint8_t>& pipes) const;

public:
	ReactionNetwork() = default;
	ReactionNetwork(const ReactionNetwork&) = delete;
	ReactionNetwork(ReactionNetwork&&) = default;

	bool insert(ReactionData& reaction);

	std::unordered_set<ConcreteReaction> getOccuringReactions(const std::vector<Reactant>& reactants) const;
	std::unordered_set<RetrosynthReaction> getRetrosynthReactions(const StructureRef& targetProduct) const;

	std::string print() const;

	void clear();

	static constexpr size_t npos = decltype(graph)::npos;
};
