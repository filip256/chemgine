#pragma once

#include <vector>

/// <summary>
/// An undirected graph stored as a lower triangular adjacency matrix.
/// Better at adding/removing edges and checking if two nodes are adjacent.
/// </summary>
template <class NodeT>
class UndirectedGraph
{
    class NeighbourIterator
    {
    private:
        const UndirectedGraph<NodeT>& owner;
        size_t                        node;
        size_t                        i;

        NeighbourIterator(const UndirectedGraph<NodeT>& owner, const size_t node) noexcept;

    public:
        NeighbourIterator(const NeighbourIterator&) = default;

        size_t       getIndex() const;
        const NodeT& operator*() const;
        const NodeT* operator->() const;
        size_t       operator++();
        bool         operator==(const NeighbourIterator& other) const;
        bool         operator!=(const NeighbourIterator& other) const;
        bool         operator==(const size_t other) const;
        bool         operator!=(const size_t other) const;

        friend class UndirectedGraph;
    };

private:
    std::vector<NodeT>                nodes;
    std::vector<std::vector<uint8_t>> edges;

public:
    UndirectedGraph()                       = default;
    UndirectedGraph(const UndirectedGraph&) = delete;
    UndirectedGraph(UndirectedGraph&&)      = default;

    void reserve(const size_t size);

    void   addEdge(const size_t i, const size_t j);
    void   removeEdge(const size_t i, const size_t j);
    size_t addNode(const NodeT& reaction);

    bool areAdjacent(const size_t i, const size_t j) const;

    size_t size() const;

    const NodeT& operator[](const size_t idx) const;
    NodeT&       operator[](const size_t idx);

    NeighbourIterator getNeighbourIterator(const size_t idx) const;

    static constexpr size_t npos = static_cast<size_t>(-1);
};

template <class NodeT>
UndirectedGraph<NodeT>::NeighbourIterator::NeighbourIterator(
    const UndirectedGraph<NodeT>& owner, const size_t node) noexcept :
    owner(owner),
    node(node),
    i(owner.size() - 1)
{
    while (i != npos && owner.areAdjacent(node, i) == false)
        --i;
}

template <class NodeT>
size_t UndirectedGraph<NodeT>::NeighbourIterator::getIndex() const
{
    return i;
}

template <class NodeT>
const NodeT& UndirectedGraph<NodeT>::NeighbourIterator::operator*() const
{
    return owner.nodes[i];
}

template <class NodeT>
const NodeT* UndirectedGraph<NodeT>::NeighbourIterator::operator->() const
{
    return &owner.nodes[i];
}

template <class NodeT>
size_t UndirectedGraph<NodeT>::NeighbourIterator::operator++()
{
    --i;
    while (i != npos && owner.areAdjacent(node, i) == false)
        --i;  // looping back allows i to become npos (u-casted -1) naturally
    return i;
}

template <class NodeT>
bool UndirectedGraph<NodeT>::NeighbourIterator::operator==(const NeighbourIterator& other) const
{
    return this->i == other.i && this->node == other.node && &this->owner == &other->owner;
}

template <class NodeT>
bool UndirectedGraph<NodeT>::NeighbourIterator::operator!=(const NeighbourIterator& other) const
{
    return this->i != other.i || this->node != other.node || &this->owner != &other->owner;
}

template <class NodeT>
bool UndirectedGraph<NodeT>::NeighbourIterator::operator==(const size_t other) const
{
    return i == other;
}

template <class NodeT>
bool UndirectedGraph<NodeT>::NeighbourIterator::operator!=(const size_t other) const
{
    return i != other;
}

// ------------------------------ //

template <class NodeT>
void UndirectedGraph<NodeT>::reserve(const size_t size)
{
    nodes.reserve(size);

    if (size > 2) {
        edges.reserve(size - 1);
    }
}

template <class NodeT>
void UndirectedGraph<NodeT>::addEdge(const size_t i, const size_t j)
{
    if (i > j)
        edges[i - 1][j] = true;
    else if (j > i)
        edges[j - 1][i] = true;
}

template <class NodeT>
void UndirectedGraph<NodeT>::removeEdge(const size_t i, const size_t j)
{
    if (i > j)
        edges[i - 1][j] = false;
    else if (j > i)
        edges[j - 1][i] = false;
}

template <class NodeT>
size_t UndirectedGraph<NodeT>::addNode(const NodeT& reaction)
{
    nodes.emplace_back(reaction);

    if (nodes.size() >= 2)
        edges.emplace_back(std::vector<uint8_t>(nodes.size() - 1, false));

    return nodes.size() - 1;
}

template <class NodeT>
bool UndirectedGraph<NodeT>::areAdjacent(const size_t i, const size_t j) const
{
    return i > j ? edges[i - 1][j] : j > i ? edges[j - 1][i] : false;
}

template <class NodeT>
size_t UndirectedGraph<NodeT>::size() const
{
    return nodes.size();
}

template <class NodeT>
const NodeT& UndirectedGraph<NodeT>::operator[](const size_t idx) const
{
    return nodes[idx];
}

template <class NodeT>
NodeT& UndirectedGraph<NodeT>::operator[](const size_t idx)
{
    return nodes[idx];
}

template <class NodeT>
UndirectedGraph<NodeT>::NeighbourIterator UndirectedGraph<NodeT>::getNeighbourIterator(const size_t idx) const
{
    return NeighbourIterator(*this, idx);
}
