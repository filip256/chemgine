#include "reactions/ReactionNetwork.hpp"

#include "global/Charset.hpp"
#include "io/Log.hpp"

ReactionNetwork::ReactionNode::ReactionNode(ReactionData& data) noexcept :
	data(data)
{}

bool ReactionNetwork::insert(const size_t current, ReactionData& reaction, size_t& firstInsert)
{
	bool matchFound = false;
	for (auto i = graph.getNeighbourIterator(current); i != npos; ++i)
	{
		if (reaction.id == i->data.id)
			continue;

		if (reaction.isSpecializationOf(i->data))
		{
			if (reaction.isEquivalentTo(i->data))
			{
				Log(this).warn("Discarded duplicate reaction with id {0}.", reaction.id);
				return false;
			}

			matchFound = insert(i.getIndex(), reaction, firstInsert);
		}
		else if (i->data.isSpecializationOf(reaction))
		{
			if (firstInsert == npos)
				firstInsert = graph.addNode(reaction);

			graph.addEdge(current, firstInsert);
			graph.addEdge(firstInsert, i.getIndex());
			graph.removeEdge(current, i.getIndex());
			reaction.setBaseReaction(graph[current].data);   // TODO: select between multiple base reactions
			i->data.setBaseReaction(reaction);
			matchFound = true;
		}
	}

	if (matchFound == false)
	{
		if (firstInsert == npos)
			firstInsert = graph.addNode(reaction);

		graph.addEdge(current, firstInsert);
		reaction.setBaseReaction(graph[current].data);
	}

	return true;
}

bool ReactionNetwork::insert(ReactionData& reaction)
{
	size_t firstInsert = npos;
	bool matchFound = false;
	for (size_t i = 0; i < topLayer.size(); ++i)
	{
		auto& topReaction = graph[topLayer[i]].data;
		if (reaction.isSpecializationOf(topReaction))
		{
			if (reaction.isEquivalentTo(topReaction))
			{
				Log(this).warn("Discarded duplicate reaction with id {0}.", reaction.id);
				return false;
			}

			if (insert(topLayer[i], reaction, firstInsert) == false)
				return false;

			matchFound = true;
		}
		else if (topReaction.isSpecializationOf(reaction))
		{
			if (firstInsert == npos)
				firstInsert = graph.addNode(reaction);

			graph.addEdge(firstInsert, topLayer[i]);
			topLayer[i] = firstInsert;
			topReaction.setBaseReaction(reaction);
			matchFound = true;
		}
	}

	if (matchFound == false)
	{
		if (firstInsert == npos)
			firstInsert = graph.addNode(reaction);
		topLayer.emplace_back(firstInsert);
	}

	return true;
}

bool ReactionNetwork::getOccurringReactions(
	const std::vector<Reactant>& reactants,
	const size_t current,
	std::unordered_set<ConcreteReaction>& result) const
{
	bool matchFound = false;
	for (auto i = graph.getNeighbourIterator(current); i != npos; ++i)
	{
		const auto& matches = i->data.generateConcreteReactantMatches(reactants);
		if (matches.empty())
			continue;

		if (getOccurringReactions(reactants, i.getIndex(), result))
		{
			matchFound = true;
			continue;
		}

		const auto products = i->data.generateConcreteProducts(reactants, matches);
		if (products.size())
		{
			result.insert(ConcreteReaction(i->data, reactants, products));
			matchFound = true;
		}
	}
	return matchFound;
}

std::unordered_set<ConcreteReaction> ReactionNetwork::getOccurringReactions(
	const std::vector<Reactant>& reactants) const
{
	std::unordered_set<ConcreteReaction> result;
	for (size_t i = 0; i < topLayer.size(); ++i)
	{
		const auto& rData = graph[topLayer[i]].data;

		const auto& matches = rData.generateConcreteReactantMatches(reactants);
		if (matches.empty())
			continue;

		if (getOccurringReactions(reactants, topLayer[i], result))
			continue;

		const auto products = rData.generateConcreteProducts(reactants, matches);
		if (products.size())
		{
			result.insert(ConcreteReaction(rData, reactants, products));
		}
	}
	return result;
}

bool ReactionNetwork::getRetrosynthReactions(
	const StructureRef& targetProduct,
	const size_t current,
	std::unordered_set<RetrosynthReaction>& result) const
{
	bool matchFound = false;
	for (auto i = graph.getNeighbourIterator(current); i != npos; ++i)
	{
		const auto& match = i->data.generateRetrosynthProductMatches(targetProduct);
		if (match.first == ReactionData::npos)
			continue;

		if (getRetrosynthReactions(targetProduct, i.getIndex(), result))
		{
			matchFound = true;
			continue;
		}

		auto reaction = i->data.generateRetrosynthReaction(targetProduct, match);
		result.emplace(std::move(reaction));
		matchFound = true;
	}
	return matchFound;
}

std::unordered_set<RetrosynthReaction> ReactionNetwork::getRetrosynthReactions(
	const StructureRef& targetProduct) const
{
	std::unordered_set<RetrosynthReaction> result;
	for (size_t i = 0; i < topLayer.size(); ++i)
	{
		const auto& rData = graph[topLayer[i]].data;

		const auto& match = rData.generateRetrosynthProductMatches(targetProduct);
		if (match.first == ReactionData::npos)
			continue;

		if (getRetrosynthReactions(targetProduct, topLayer[i], result))
			continue;

		auto reaction = rData.generateRetrosynthReaction(targetProduct, match);
		result.emplace(std::move(reaction));
	}
	return result;
}

void ReactionNetwork::print(const size_t current, TextBlock& block, size_t& y, std::vector<uint8_t>& pipes) const
{
	pipes.emplace_back(true);

	for (auto i = graph.getNeighbourIterator(current); i != npos; ++i)
	{
		++y;

		for (size_t i = 0; i < pipes.size() - 1; ++i)
			if (pipes[i])
				block[y][i * 3] = ASCII::LineV;

		auto peek = i;
		if (++peek != npos)
			block[y].insert((pipes.size() - 1) * 3, { ASCII::JunctionRight, ASCII::LineH, ASCII::LineH });
		else
		{
			block[y].insert((pipes.size() - 1) * 3, { ASCII::CornerBottomLeft, ASCII::LineH, ASCII::LineH });
			pipes.back() = false;
		}

		block[y].insert(pipes.size() * 3, i->data.getHRTag());
		print(i.getIndex(), block, y, pipes);
	}

	pipes.pop_back();
}

std::string ReactionNetwork::print() const
{
	TextBlock block;

	size_t y = 0;
	for (size_t i = 0; i < topLayer.size(); ++i)
	{
		block[y].insert(0, graph[topLayer[i]].data.getHRTag());
		std::vector<uint8_t> pipesAlloc;
		print(topLayer[i], block, y, pipesAlloc);
		++y;
	}

	return block.toString();
}

void ReactionNetwork::clear()
{
	graph.clear();
	topLayer.clear();
}
