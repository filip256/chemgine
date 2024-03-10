#include "LayerIterator.hpp"

LayerIterator::LayerIterator(
	const LayerType layer,
	const std::unordered_set<Reactant>::const_iterator it,
    const std::unordered_set<Reactant>::const_iterator end
) noexcept :
	layer(layer),
	it(it),
    end(end)
{}

const Reactant& LayerIterator::operator*() const 
{
    return *it;
}

const Reactant* LayerIterator::operator->() const 
{
    return &(*it);
}

LayerIterator& LayerIterator::operator++()
{
    while(it != end && it->layer != layer)
        ++it;
    return *this;
}

bool LayerIterator::operator==(const LayerIterator& other) const 
{
    return this->it == other.it;
}

bool LayerIterator::operator!=(const LayerIterator& other) const 
{
    return this->it != other.it;
}