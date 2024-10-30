#pragma once

#include <vector>

/// <summary>
/// A directed graph stored as an adjacency matrix.
/// Better at adding/removing edges and checking if two nodes are adjacent.
/// </summary>
template<class NodeT>
class DirectedGraph
{
	class NeighbourIterator
	{
	private:
		const DirectedGraph<NodeT>& owner;
		size_t node;
		size_t i;

		NeighbourIterator(
			const DirectedGraph<NodeT>& owner,
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

		friend class DirectedGraph;
	};

private:
	std::vector<NodeT> nodes;
	std::vector<std::vector<uint8_t>> edges;

public:
	DirectedGraph() = default;
	DirectedGraph(const DirectedGraph&) = delete;
	DirectedGraph(DirectedGraph&&) = default;

	void reserve(const size_t size);

	/// <summary>
	/// Complexity: O(1)
	/// </summary>
	void addEdge(const size_t i, const size_t j);
	/// <summary>
	/// Complexity: O(1)
	/// </summary>
	void removeEdge(const size_t i, const size_t j);
	/// <summary>
	/// Complexity: O(n)
	/// </summary>
	size_t addNode(const NodeT& reaction);
	/// <summary>
	/// Complexity: O(1)
	/// </summary>
	bool areAdjacent(const size_t i, const size_t j) const;

	size_t size() const;

	void clear();

	const NodeT& operator[](const size_t idx) const;
	NodeT& operator[](const size_t idx);

	NeighbourIterator getNeighbourIterator(const size_t idx) const;

	static constexpr size_t npos = static_cast<size_t>(-1);
};



template<class NodeT>
DirectedGraph<NodeT>::NeighbourIterator::NeighbourIterator(
	const DirectedGraph<NodeT>& owner,
	const size_t node
) noexcept :
	owner(owner),
	node(node),
	i(owner.size() - 1)
{
	while (i != npos && owner.areAdjacent(node, i) == false)
		--i;
}

template<class NodeT>
size_t DirectedGraph<NodeT>::NeighbourIterator::getIndex() const
{
	return i;
}

template<class NodeT>
const NodeT& DirectedGraph<NodeT>::NeighbourIterator::operator*() const
{
	return owner.nodes[i];
}

template<class NodeT>
const NodeT* DirectedGraph<NodeT>::NeighbourIterator::operator->() const
{
	return &owner.nodes[i];
}

template<class NodeT>
size_t DirectedGraph<NodeT>::NeighbourIterator::operator++()
{
	--i;
	while (i != npos && owner.areAdjacent(node, i) == false)
		--i;   // looping back allows i to become npos (u-casted -1) naturally 
	return i;
}

template<class NodeT>
bool DirectedGraph<NodeT>::NeighbourIterator::operator==(const NeighbourIterator& other) const
{
	return this->i == other.i && this->node == other.node && &this->owner == &other->owner;
}

template<class NodeT>
bool DirectedGraph<NodeT>::NeighbourIterator::operator!=(const NeighbourIterator& other) const
{
	return this->i != other.i || this->node != other.node || &this->owner != &other->owner;
}

template<class NodeT>
bool DirectedGraph<NodeT>::NeighbourIterator::operator==(const size_t other) const
{
	return i == other;
}

template<class NodeT>
bool DirectedGraph<NodeT>::NeighbourIterator::operator!=(const size_t other) const
{
	return i != other;
}

// ------------------------------ //

template<class NodeT>
void DirectedGraph<NodeT>::reserve(const size_t size)
{
	nodes.reserve(size);
	edges.reserve(size);
}

template<class NodeT>
void DirectedGraph<NodeT>::addEdge(const size_t i, const size_t j)
{
	edges[i][j] = true;
}

template<class NodeT>
void DirectedGraph<NodeT>::removeEdge(const size_t i, const size_t j)
{
	edges[i][j] = false;
}

template<class NodeT>
size_t DirectedGraph<NodeT>::addNode(const NodeT& reaction)
{
	nodes.emplace_back(reaction);
	
	for (size_t i = 0; i < edges.size(); ++i)
		edges[i].emplace_back(false);
	edges.emplace_back(std::vector<uint8_t>(nodes.size(), false));

	return nodes.size() - 1;
}

template<class NodeT>
bool DirectedGraph<NodeT>::areAdjacent(const size_t i, const size_t j) const
{
	return edges[i][j];
}

template<class NodeT>
size_t DirectedGraph<NodeT>::size() const
{
	return nodes.size();
}

template<class NodeT>
void DirectedGraph<NodeT>::clear()
{
	nodes.clear();
	edges.clear();
}

template<class NodeT>
const NodeT& DirectedGraph<NodeT>::operator[](const size_t idx) const
{
	return nodes[idx];
}

template<class NodeT>
NodeT& DirectedGraph<NodeT>::operator[](const size_t idx)
{
	return nodes[idx];
}

template<class NodeT>
DirectedGraph<NodeT>::NeighbourIterator DirectedGraph<NodeT>::getNeighbourIterator(const size_t idx) const
{
	return NeighbourIterator(*this, idx);
}