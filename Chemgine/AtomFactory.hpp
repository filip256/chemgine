#pragma once

#include "Atom.hpp"
#include "Symbol.hpp"

#include <memory>

class AtomFactory
{
public:
	static std::unique_ptr<const Atom> get(const Symbol& symbol);
};
