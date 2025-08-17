#pragma once

#include <vector>

/// <summary>
/// A directed graph stored as adjacency lists.
/// Better at inserting nodes and iterating over the neighbours of a node.
/// </summary>
template<class NodeT>
class SparseDirectedGraph
{
	class NeighbourIterator
	{
	private:
		const SparseDirectedGraph<NodeT>& owner;
		const std::vector<size_t>& edges;
		size_t i;

		NeighbourIterator(
			const SparseDirectedGraph<NodeT>& owner,
			const size_t node
		) noexcept;

	public:
		NeighbourIterator(const NeighbourIterator&) = default;

		size_t getIndex() const;
		const NodeT& operator*() const;
		const NodeT* operator->() const;
		size_t operator++();
		bool operator==(const NeighbourIterator& other) const;
		bool operator!=(const NeighbourIterator& other) const;
		bool operator==(const size_t other) const;
		bool operator!=(const size_t other) const;

		friend class SparseDirectedGraph;
	};

private:
	std::vector<NodeT> nodes;
	std::vector<std::vector<size_t>> edges;

public:
	SparseDirectedGraph() = default;
	SparseDirectedGraph(const SparseDirectedGraph&) = delete;
	SparseDirectedGraph(SparseDirectedGraph&&) = default;

	void reserve(const size_t size);

	/// <summary>
	/// Complexity: O(n)
	/// </summary>
	void addEdge(const size_t i, const size_t j);
	/// <summary>
	/// Invalidates iterators.
	/// Complexity: O(n)
	/// </summary>
	void removeEdge(const size_t i, const size_t j);
	/// <summary>
	/// Complexity: O(1)
	/// </summary>
	size_t addNode(const NodeT& reaction);
	/// <summary>
	/// Complexity: O(n)
	/// </summary>
	bool areAdjacent(const size_t i, const size_t j) const;

	size_t size() const;

	const NodeT& operator[](const size_t idx) const;
	NodeT& operator[](const size_t idx);

	NeighbourIterator getNeighbourIterator(const size_t idx) const;

	static constexpr size_t npos = static_cast<size_t>(-1);
};



template<class NodeT>
SparseDirectedGraph<NodeT>::NeighbourIterator::NeighbourIterator(
	const SparseDirectedGraph<NodeT>& owner,
	const size_t node
) noexcept :
	owner(owner),
	edges(owner.edges[node]),
	i(edges.size() - 1)
{}

template<class NodeT>
size_t SparseDirectedGraph<NodeT>::NeighbourIterator::getIndex() const
{
	return edges[i];
}

template<class NodeT>
const NodeT& SparseDirectedGraph<NodeT>::NeighbourIterator::operator*() const
{
	return owner.nodes[getIndex()];
}

template<class NodeT>
const NodeT* SparseDirectedGraph<NodeT>::NeighbourIterator::operator->() const
{
	return &owner.nodes[getIndex()];
}

template<class NodeT>
size_t SparseDirectedGraph<NodeT>::NeighbourIterator::operator++()
{
	return --i;
}

template<class NodeT>
bool SparseDirectedGraph<NodeT>::NeighbourIterator::operator==(const NeighbourIterator& other) const
{
	return this->i == other.i && this->node == other.node && &this->owner == &other->owner;
}

template<class NodeT>
bool SparseDirectedGraph<NodeT>::NeighbourIterator::operator!=(const NeighbourIterator& other) const
{
	return this->i != other.i || this->node != other.node || &this->owner != &other->owner;
}

template<class NodeT>
bool SparseDirectedGraph<NodeT>::NeighbourIterator::operator==(const size_t other) const
{
	return i == other;
}

template<class NodeT>
bool SparseDirectedGraph<NodeT>::NeighbourIterator::operator!=(const size_t other) const
{
	return i != other;
}

// ------------------------------ //

template<class NodeT>
void SparseDirectedGraph<NodeT>::reserve(const size_t size)
{
	nodes.reserve(size);
	edges.reserve(size);
}

template<class NodeT>
void SparseDirectedGraph<NodeT>::addEdge(const size_t i, const size_t j)
{
	auto& iEdges = edges[i];
	for (size_t e = 0; e < iEdges.size(); ++e)
		if (iEdges[e] == j)
			return;
	iEdges.emplace_back(j);
}

template<class NodeT>
void SparseDirectedGraph<NodeT>::removeEdge(const size_t i, const size_t j)
{
	auto& iEdges = edges[i];
	for (size_t e = 0; e < iEdges.size(); ++e)
		if (iEdges[e] == j)
		{
			iEdges.erase(iEdges.begin() + e);
			return;
		}
}

template<class NodeT>
size_t SparseDirectedGraph<NodeT>::addNode(const NodeT& reaction)
{
	nodes.emplace_back(reaction);
	edges.emplace_back();

	return nodes.size() - 1;
}

template<class NodeT>
bool SparseDirectedGraph<NodeT>::areAdjacent(const size_t i, const size_t j) const
{
	const auto& iEdges = edges[i];
	for (size_t e = 0; e < iEdges.size(); ++e)
		if (iEdges[e] == j)
			return true;
	return false;
}

template<class NodeT>
size_t SparseDirectedGraph<NodeT>::size() const
{
	return nodes.size();
}

template<class NodeT>
const NodeT& SparseDirectedGraph<NodeT>::operator[](const size_t idx) const
{
	return nodes[idx];
}

template<class NodeT>
NodeT& SparseDirectedGraph<NodeT>::operator[](const size_t idx)
{
	return nodes[idx];
}

template<class NodeT>
SparseDirectedGraph<NodeT>::NeighbourIterator SparseDirectedGraph<NodeT>::getNeighbourIterator(const size_t idx) const
{
	return NeighbourIterator(*this, idx);
}
