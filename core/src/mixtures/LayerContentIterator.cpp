#include "mixtures/LayerContentIterator.hpp"

LayerContentIterator::LayerContentIterator(
	const LayerType layer,
	const ReactantSet::const_iterator it,
    const ReactantSet::const_iterator end
) noexcept :
	layer(layer),
	it(it),
    end(end)
{
    while (this->it != end && this->it->first.layer != layer)
        ++(this->it);
}

const Reactant& LayerContentIterator::operator*() const 
{
    return it->second;
}

const Reactant* LayerContentIterator::operator->() const 
{
    return &(it->second);
}

LayerContentIterator& LayerContentIterator::operator++()
{
    ++it;
    while(it != end && it->first.layer != layer)
        ++it;
    return *this;
}

bool LayerContentIterator::operator==(const LayerContentIterator& other) const 
{
    return this->it == other.it;
}

bool LayerContentIterator::operator!=(const LayerContentIterator& other) const 
{
    return this->it != other.it;
}
