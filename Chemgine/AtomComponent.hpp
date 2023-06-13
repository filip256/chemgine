#pragma once

#include "Atom.hpp"
#include "CompositeComponent.hpp"

class AtomComponent : public Atom
{
private:
	const CompositeComponent& composite;

public:
	AtomComponent(const CompositeComponent& composite, const AtomIdType id);
};