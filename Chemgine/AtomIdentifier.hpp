#pragma once

#include "Atom.hpp"
#include "CompositeComponent.hpp"

class AtomIdentifier : public Atom
{
private:
	const CompositeComponent& composite;

public:
	AtomIdentifier(const CompositeComponent& composite, const AtomIdType id);
};