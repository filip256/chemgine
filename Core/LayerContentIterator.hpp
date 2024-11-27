#pragma once

#include "LayerType.hpp"
#include "Reactant.hpp"
#include "ReactantSet.hpp"

class Layer;

class LayerContentIterator
{
private:
	const LayerType layer;
	ReactantSet::const_iterator it;
	const ReactantSet::const_iterator end;

	LayerContentIterator(
		const LayerType layer,
		const ReactantSet::const_iterator it,
		const ReactantSet::const_iterator end
	) noexcept;

public:
	LayerContentIterator(const LayerContentIterator&) = default;

	const Reactant& operator*() const;
	const Reactant* operator->() const;
	LayerContentIterator& operator++();
	bool operator==(const LayerContentIterator& other) const;
	bool operator!=(const LayerContentIterator& other) const;

	friend class Layer;
};
