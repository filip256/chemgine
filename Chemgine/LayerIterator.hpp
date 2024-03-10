#pragma once

#include "LayerType.hpp"
#include "Reactant.hpp"
#include "ReactantSet.hpp"

#include <unordered_set>

class Layer;

class LayerIterator
{
private:
	const LayerType layer;
	ReactantSet::const_iterator it;
	const ReactantSet::const_iterator end;

	LayerIterator(
		const LayerType layer,
		const ReactantSet::const_iterator it,
		const ReactantSet::const_iterator end
	) noexcept;

public:
	LayerIterator(const LayerIterator&) = default;

	const Reactant& operator*() const;
	const Reactant* operator->() const;
	LayerIterator& operator++();
	bool operator==(const LayerIterator& other) const;
	bool operator!=(const LayerIterator& other) const;

	friend class Layer;
};