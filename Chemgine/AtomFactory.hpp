#pragma once

#include "Atom.hpp"
#include "Symbol.hpp"

class AtomFactory
{
public:
	static Atom* get(const AtomId id);
	static Atom* get(const Symbol symbol);
};
