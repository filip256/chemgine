#pragma once

#include "ReactionData.hpp"
#include "DirectedGraph.hpp"
#include "TextBlock.hpp"
#include "ConcreteReaction.hpp"

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

	bool getConcreteReactions(
		const std::vector<Reactant>& reactants,
		const size_t current,
		std::unordered_set<ConcreteReaction>& result) const;

	void print(const size_t current, TextBlock& block, size_t& y, std::vector<uint8_t>& pipes) const;

public:
	ReactionNetwork() = default;
	ReactionNetwork(const ReactionNetwork&) = delete;

	bool insert(ReactionData& reaction);

	std::unordered_set<ConcreteReaction> getConcreteReactions(const std::vector<Reactant>& reactants) const;

	std::string print() const;

	static constexpr size_t npos = decltype(graph)::npos;
};