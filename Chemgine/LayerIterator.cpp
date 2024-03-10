#include "LayerIterator.hpp"

LayerIterator::LayerIterator(
	const LayerType layer,
	const ReactantSet::const_iterator it,
    const ReactantSet::const_iterator end
) noexcept :
	layer(layer),
	it(it),
    end(end)
{}

const Reactant& LayerIterator::operator*() const 
{
    return it->second;
}

const Reactant* LayerIterator::operator->() const 
{
    return &(it->second);
}

LayerIterator& LayerIterator::operator++()
{
    while(it != end && it->first.layer != layer)
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