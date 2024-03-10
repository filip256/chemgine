#pragma once

#include "Reactant.hpp"

class ReactantSet;

class ReactantRef
{
private:
	Ref<const ReactantSet> owner;

public:
	const ReactantId id;

	ReactantRef(const Reactant& reactant) noexcept;
	ReactantRef(const ReactantRef&) = default;
};
